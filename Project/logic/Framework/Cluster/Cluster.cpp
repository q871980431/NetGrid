/*
 * File:	Cluster.cpp
 * Author:	xuping
 * 
 * Created On 2019/6/17 20:00:12
 */

#include "Cluster.h"
#include "test.pb.h"
#include "XmlReader.h"
#include "IHarbor.h"
#include "ClusterInfo.h"
#include "Tools.h"
#include "Tools_Net.h"
#include "CommonMsgDefine.h"

Cluster * Cluster::s_self = nullptr;
IKernel * Cluster::s_kernel = nullptr;
IHarbor	* Cluster::s_harbor = nullptr;
ClusterConntroller * Cluster::s_clusterConroller = nullptr;
std::string Cluster::s_masterIp;
s32			Cluster::s_masterPort;
ClusterNodeInfoMap	Cluster::s_clusterNodeMap;

bool Cluster::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	
	SearchRequest searchRequest;
	searchRequest.set_query("actor");
	searchRequest.set_page_num(1);
	std::string content;
	if (!searchRequest.SerializePartialToString(&content))
	{
		ASSERT(false, "error");
		return false;
	}
	SearchRequest temp;
	if (!temp.ParseFromArray(content.c_str(), content.size()))
	{
		ASSERT(false, "error");
		return false;
	}
	int page = temp.page_num();
	ECHO("PAGE:%d", page);
	
    return true;
}

bool Cluster::Launched(IKernel *kernel)
{
	FIND_MODULE(s_harbor, Harbor);
	auto *harborAddr = s_harbor->GetHarborAddr();
	bool master = harborAddr->nodeType == NODE_TYPE::NODE_TYPE_MASTER;

	s_clusterConroller = NEW ClusterConntroller();
	if (!s_clusterConroller->Init(s_kernel, master))
		return false;

	if (!LoadClusterConfig(kernel))
		return false;

	if (master)
	{
		s_harbor->AddListener(this);
	}
	else
	{
		s_harbor->ConnectHarbor(core::NODE_TYPE_MASTER, 1, s_masterIp.c_str(), s_masterPort);
		s_harbor->RegisterMessage(core::NODE_MSG_ID_CLUSTER_ADHOC, Cluster::OnRecvClusterAdhocMsg);
	}

    return true;
}

void Cluster::OnOpen(s8 type, s8 nodeId, const char *ip, s32 port)
{
	s_clusterConroller->AddClusterNode(type, nodeId);
	s64 channel = GetSessionChannel(type, nodeId);
	auto iter = s_clusterNodeMap.find(channel);
	ASSERT(iter == s_clusterNodeMap.end(), "error");
	if (iter == s_clusterNodeMap.end())
	{
		ClusterNodeInfo nodeInfo;
		nodeInfo.nodeType = type;
		nodeInfo.nodeId = nodeId;
		nodeInfo.ip = ip;
		nodeInfo.port = port;

		core::NODE_MSG_CLUSTER_ADHOC_NETWORK discover;
		discover.nodeType = type;
		discover.nodeId = nodeId;
		discover.ipAddr = tools::GetIpV4Addr(ip);
		discover.port = port;

		core::NODE_MSG_CLUSTER_ADHOC_NETWORK target;
		for (auto iter = s_clusterNodeMap.begin(); iter != s_clusterNodeMap.end(); iter++)
		{
			if (iter->first < channel)
				s_harbor->SendMessage(iter->second.nodeType, iter->second.nodeId, core::NODE_MSG_ID_CLUSTER_ADHOC, &discover, sizeof(discover));
			else
			{
				target.nodeType = iter->second.nodeType;
				target.nodeId = iter->second.nodeId;
				target.ipAddr = tools::GetIpV4Addr(iter->second.ip.c_str());
				target.port = iter->second.port;

				s_harbor->SendMessage(type, nodeId, core::NODE_MSG_ID_CLUSTER_ADHOC, &target, sizeof(target));
			}
		}

		s_clusterNodeMap.emplace(channel, nodeInfo);
	}
}

void Cluster::OnClose(s8 type, s8 nodeId)
{
	s_clusterConroller->DelClusterNode(type, nodeId);
	s64 channel = GetSessionChannel(type, nodeId);
	auto iter = s_clusterNodeMap.find(channel);
	ASSERT(iter != s_clusterNodeMap.end(), "error");
	if (iter != s_clusterNodeMap.end())
		s_clusterNodeMap.erase(iter);
}

void Cluster::OnRecvClusterAdhocMsg(s8 type, s8 nodeId, const char *buff, s32 len)
{
	const NODE_MSG_CLUSTER_ADHOC_NETWORK *msg = CAST_DATA_PTR(NODE_MSG_CLUSTER_ADHOC_NETWORK, buff, len);
	if (msg)
	{
		sockaddr_in sockAddr;
		sockAddr.sin_addr.s_addr = msg->ipAddr;
		char ip[IP_LEN] = { 0 };
		const char * tmpIp = tools::GetSocketIp(sockAddr);
		tools::SafeStrcpy(ip, sizeof(ip), tmpIp, strlen(tmpIp));
		TRACE_LOG("recv cluster adhocmsg, nodetype:%d, nodeid:%d, ip:%s, port:%d", msg->nodeType, msg->nodeId, ip, msg->port);

		s_harbor->ConnectHarbor(msg->nodeType, msg->nodeId, ip, msg->port, true);
	}
}

bool Cluster::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

void Cluster::OnClusterNodesTypeChange(IKernel *kernel, const void *context, s32 size)
{
	s_clusterConroller->OnClusterNodesTypeChange(kernel, context, size);
}

bool Cluster::LoadClusterConfig(IKernel *kernel)
{
	XmlReader reader;
	if (!reader.LoadFile(s_kernel->GetCoreFile()))
		return false;
	IXmlObject *cluster = reader.Root()->GetFirstChrild("cluster");
	if (cluster == nullptr)
		return false;
	s_masterIp = cluster->GetAttribute_Str("master_ip");
	s_masterPort = cluster->GetAttribute_S32("port");

	IXmlObject *clusterNode = cluster->GetFirstChrild("node");
	while (clusterNode)
	{
		const char *name = clusterNode->GetAttribute_Str("name");
		s8 type = clusterNode->GetAttribute_S8("type");
		s_clusterConroller->InitClusterNode(type, name);

		clusterNode = clusterNode->GetNextSibling();
	}
	s_clusterConroller->InitCompelate();

	return true;
}


s64 Cluster::GetSessionChannel(s8 type, s8 nodeId)
{
	tools::_KEY_INT64 channel;
	channel.hVal = type;
	channel.lVal = nodeId;
	return channel.val;
}