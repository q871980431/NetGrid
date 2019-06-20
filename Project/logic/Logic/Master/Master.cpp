/*
 * File:	Master.cpp
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:14:49
 */

#include "Master.h"
#include "Tools.h"
#include "XmlReader.h"
#include "ServiceGroup.h"
#include "Tools_Net.h"
#include "CommonMsgDefine.h"

Master * Master::s_self = nullptr;
IKernel * Master::s_kernel = nullptr;
IHarbor * Master::s_harbor = nullptr;
ServiceGroup * Master::s_serviceGroup = nullptr;
NodeMap   Master::s_nodeMap;
SlaveMap Master::s_slaves;

bool Master::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	s_serviceGroup = NEW ServiceGroup();

	const char *configFile = kernel->GetConfigFile();
	if (!LoadConfigFile(configFile))
		return false;
	s_serviceGroup->StartService(s_kernel);

    return true;
}

bool Master::Launched(IKernel *kernel)
{
    FIND_MODULE(s_harbor, Harbor);

    s_harbor->AddListener(this);

    return true;
}

bool Master::Destroy(IKernel *kernel)
{
	SAFE_DELETE(s_serviceGroup);
    DEL this;
    return true;
}

void Master::OnOpen(s32 type, s32 nodeId, const char *ip, s16 port)
{
    tools::KEYINT64 key;
	if (type == core::NODE_TYPE_SLAVE)
	{
		NodeInfo node;
		node.nodeId = nodeId;
		node.state = core::NODE_STATUS_CONNECT;

		auto ret = s_slaves.insert(std::make_pair(node.nodeId, node));
		ASSERT(ret.second, "error");
		return;
	}

    core::NODE_MSG_NODE_DISCOVER discover;
    discover.ipAddr = tools::GetIpV4Addr(ip);
    discover.port = port;

    for (auto iter = s_nodeMap.begin(); iter != s_nodeMap.end(); iter++)
    {
        key.val = *iter;
        if (key.hVal > type)
        {
            s_harbor->SendMessage(key.hVal, key.lVal, core::NODE_MSG_CONNECT_HARBOR, &discover, sizeof(discover));
        }
        else
        {
            if (key.hVal == type && key.lVal > nodeId)
            {
                s_harbor->SendMessage(key.hVal, key.lVal, core::NODE_MSG_CONNECT_HARBOR, &discover, sizeof(discover));
            }
        }
    }

	key.hVal = type;
	key.lVal = nodeId;
	auto ret = s_nodeMap.insert(key.val);
    ASSERT(ret.second, "error");

}

void Master::OnClose(s32 type, s32 nodeId)
{

}

bool Master::StartServiceNode(s32 slave, const char *name, s8 type, s32 id, const char *cmd)
{
	if (slave == core::INVALID)
		slave = s_self->GetBestNode(type);
	auto iter = s_slaves.find(slave);
	ASSERT(iter != s_slaves.end(), "error");
	if (iter != s_slaves.end())
	{

	}

	return true;
}

void Master::OnRecvNodeHasReadyMsg(s32 type, s32 nodeId, const char *buff, s32 len)
{
	core::NODE_STATUS_UPDATE *msg = (core::NODE_STATUS_UPDATE *)buff;
	ASSERT(sizeof(core::NODE_STATUS_UPDATE) == len, "error");
	if (type == core::NODE_TYPE_SLAVE)
	{
		auto iter = s_slaves.find(nodeId);
		ASSERT(iter != s_slaves.end(), "error");
		if (iter != s_slaves.end())
		{
			iter->second.state = msg->status;
		}
	}else
	{

	}
}



bool Master::LoadConfigFile(const char *path)
{
	XmlReader doc;
	if (!doc.LoadFile(path))
	{
		ASSERT(false, "error, don't find file, path = %s", path);

		return false;
	}
	IXmlObject *serviceGroup = doc.Root()->GetFirstChrild("service_group");
	if (serviceGroup != nullptr)
	{
		IXmlObject *service = serviceGroup->GetFirstChrild("service");
		while (service)
		{
			ServiceConfig info;
			info.external = false;
			info.type = service->GetAttribute_S8("type");
			info.name = service->GetAttribute_Str("name");
			info.cmd = service->GetAttribute_Str("cmd");
			info.slave = service->GetAttribute_S8("slave");
			info.min = service->GetAttribute_S16("min");
			info.max = service->GetAttribute_S16("max");
			if (info.min < 1)
				info.min = 1;
			if (info.max < 1)
				info.max = 1;

			s_serviceGroup->AddService(&info);

			service = service->GetNextSibling();
		}
	}

	return true;
}

s32 Master::GetBestNode(s8 type)
{
	s32 tmp = 0;
	s32 ret = 0;
	for (auto iter : s_slaves)
	{
		if (tmp == 0 || tmp < iter.second.weighted)
		{
			tmp = iter.second.weighted;
			ret = iter.first;
		}
	}

	return ret;
}
