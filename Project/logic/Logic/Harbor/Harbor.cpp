/*
 * File:	Harbor.cpp
 * Author:	XuPing
 * 
 * Created On 2017/9/3 22:51:33
 */

#include "Harbor.h"
#include "FrameworkDefine.h"
#include "XmlReader.h"
#include "Convert.h"
#include "HarborSession.h"
#include "CommonMsgDefine.h"
#include "Tools.h"
#include "Tools_Net.h"
#ifdef LINUX
#include <arpa/inet.h>
#endif

Harbor		  * Harbor::s_self = nullptr;
IKernel		  * Harbor::s_kernel = nullptr;
NodeListeners	Harbor::s_listeners;
MsgCallBackMap	Harbor::s_callbacks;
s32				Harbor::s_nodeType;
s32				Harbor::s_nodeId;
s32				Harbor::s_buffer;
s16				Harbor::s_port;
MasterInfo		Harbor::s_master;
NodeMap			Harbor::s_nodes;
BufferDesMap	Harbor::s_buffDes;
SessionMap		Harbor::s_sessions;
HarborAddr		Harbor::s_harborAddr;
SessionMap		Harbor::s_clientSessions;

#include <limits>
bool Harbor::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;

    const char *nodeType = s_kernel->GetCmdArg("type");
    const char *nodeId = s_kernel->GetCmdArg("id");
    const char *port = s_kernel->GetCmdArg("port");
    ASSERT(nodeType != nullptr, "error");
    ASSERT(nodeId != nullptr, "error");
    ASSERT(port != nullptr, "error");
	s_harborAddr.areaId = HARBOR_AREA_SELF;
    s_harborAddr.nodeType = tools::StrToInt8(nodeType);
    s_harborAddr.nodeId = tools::StrToInt8(nodeId);
    s_port = tools::StrToInt32(port);
    const char *corePath = s_kernel->GetCoreFile();
    LoadConfigFile(corePath);


    return true;
}

bool Harbor::Launched(IKernel *kernel)
{
    s_kernel->CreateNetListener("0", s_port, this);
    //RegisterMessage(core::NODE_MSG_CLUSTER_ADHOC, OnRecvMasterConnectHarborMsg);

    return true;
}

bool Harbor::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

void Harbor::AddListener(INodeListener *listener)
{
    s_listeners.push_back(listener);
}

void Harbor::RegisterMessage(s32 messageId, const CALL_BACK fun)
{
    auto ret = s_callbacks.insert(std::make_pair(messageId, fun));
    ASSERT(ret.second, "insert error");
}

void Harbor::Sendbuff(s8 type, s8 nodeId, const void *buff, s32 len)
{
    s64 key = GetChannel(type, nodeId);
    auto iter = s_sessions.find(key);
    if (iter != s_sessions.end())
        iter->second->SendBuff((const char *)buff, len);
}

void Harbor::SendMessage(s8 type, s8 nodeId, s32 messageId, const void *buff, s32 len)
{
    s64 key = GetChannel(type, nodeId);
    auto iter = s_sessions.find(key);
    if (iter != s_sessions.end())
        iter->second->SendMsg(messageId, (const char *)buff, len);
}

ITcpSession * Harbor::CreateSession()
{
    HarborSession *session = NEW HarborSession(this, s_kernel);
    return session;
}

bool Harbor::ConnectHarbor(s8 type, s8 nodeId, const char *ip, s32 port, bool force /* = false */)
{
	s64 key = GetChannel(type, nodeId);
	s64 myKey = GetChannel(s_harborAddr.nodeType, s_harborAddr.nodeId);
	if (type != core::NODE_TYPE_MASTER)
	{
		if (key < myKey)
		{
			ERROR_LOG("Connect Harbor Error, NodeType:%d, NodeId:%d, Target NodeType:%d, NodeId:%d", s_nodeType, s_nodeId, type, nodeId);
			return false;
		}
	}
	auto iter = s_clientSessions.find(key);
	if (iter != s_clientSessions.end())
	{
		TRACE_LOG("connect harbor error, had try connect, but this is force connect, so force connect");
		if (!force)
		{
			TRACE_LOG("Connect Harbor Error, Target NodeType:%d, NodeId:%d, Had Try Connect", type, nodeId);
			return false;
		}
		DisconnectHarbor(type, nodeId, "connect harbor by force");
	}
	HarborClientSession *session = NEW HarborClientSession(s_kernel, s_self, ip, port, key);
	s_kernel->CreateNetSession(ip, port, session);
	session->SetConnecting();
	s_clientSessions.emplace(key, session);

	return true;
}

bool Harbor::DisconnectHarbor(s8 type, s8 nodeId, const char *reason)
{
	s64 key = GetChannel(type, nodeId);
	auto iter = s_clientSessions.find(key);
	if (iter != s_clientSessions.end())
	{
		iter->second->Close(reason);
		return true;
	}

	return false;
}

HarborClientSession::~HarborClientSession()
{
    if (_timer != nullptr)
    {
        _kernel->KillTimer(_timer);
    }
}

void Harbor::OnOpen(HarborSession *session)
{
    NODE_MSG_HANDSHAKE msg;
    msg.nodeType = s_harborAddr.nodeType;
    msg.nodeId = s_harborAddr.nodeId;
    msg.port = s_port;
    core::MessageHead head;
    head.messageId = core::NODE_MSG_ID_HAND_SHAKE;
    head.len = sizeof(msg) + sizeof(head);

    session->SendBuff((const char *)&head, sizeof(head));
    session->SendBuff((const char *)&msg, sizeof(msg));
}

void Harbor::OnRecv(HarborSession *session, s32 messageId, const char *buff, s32 len)
{
    s32 type = session->GetNodeType();
    s32 nodeId = session->GetNodeID();

	if (messageId == core::NODE_MSG_ID_HAND_SHAKE)
		OnRecvHarborHandshakeMsg(session, buff, len);
    else
    {
        auto iter = s_callbacks.find(messageId);
        if (iter != s_callbacks.end())
            iter->second(type, nodeId, buff, len);
    }
}

void Harbor::OnClose(HarborSession *session)
{
    auto endIter = s_listeners.end();
    s32 type = session->GetNodeType();
    s32 nodeId = session->GetNodeID();
    if ( type != 0 || nodeId != 0)
    {
		session->SetNodeInfo(0, 0);
		const char *name = GetNodeNameByNodeType(type);
		DelayOpenTimer *timer = session->GetDelayTimer();
		if (timer != nullptr)
		{
			s_kernel->KillTimer(timer);
			session->SetDelayTimer(nullptr);
		}

		TRACE_LOG("Node :%s Close, type = %d, id = %d", name, type, nodeId);
        for (auto iter = s_listeners.begin(); iter != endIter; iter++)
            (*iter)->OnClose(type, nodeId);

		s64 key = GetChannel(type, nodeId);
		auto iter = s_sessions.find(key);
		ASSERT(iter != s_sessions.end(), "error");
		if (iter != s_sessions.end())
			s_sessions.erase(iter);

    }
}

void Harbor::OnHarborClientSessionRelease(HarborClientSession *session)
{
	HostInfo *hostInfo = session->GetHostInfo();
	TRACE_LOG("client session release, host ip:%s, port:%d", hostInfo->ip.GetString(), hostInfo->port);
	s64 key = session->GetChannel();
	auto iter = s_clientSessions.find(key);
	if (iter == s_clientSessions.end())
	{
		ASSERT(false, "error");
		return;
	}
	//auto sessionIter = s_sessions.find(key);
	//ASSERT(sessionIter == s_sessions.end(), "error");
	DEL session;
	s_clientSessions.erase(iter);
}

void Harbor::OnTimerSessionDelayOpen(HarborSession *session)
{
	session->SetDelayTimer(nullptr);
	s8 nodeType = session->GetNodeType();
	s8 nodeId = session->GetNodeID();
	const char *sessionIp = session->GetSessionIp();
	s32 sessionPort = session->GetSessionPort();
	const char *name = GetNodeNameByNodeType(nodeType);
	TRACE_LOG("Node :%s OnOpen, type = %d, id = %d, ip = %s, port = %d", name, nodeType, nodeId, sessionIp, sessionPort);

	auto endIter = s_listeners.end();
	for (auto iter = s_listeners.begin(); iter != endIter; iter++)
		(*iter)->OnOpen(nodeType, nodeId, sessionIp, sessionPort );
}

bool Harbor::LoadConfigFile(const char *path)
{
    XmlReader reader;
    if (!reader.LoadFile(path))
    {
        ASSERT(false, "don't find file = %s", path);
        return false;
    }
    IXmlObject *root = reader.Root();
    //IXmlObject *master = root->GetFirstChrild("master");
    //ASSERT(master != nullptr, "error");
    //s_master.ip = master->GetAttribute_Str("ip");
    //s_master.port = master->GetAttribute_S16("port");
    IXmlObject *harbor = root->GetFirstChrild("harbor");
    ASSERT(harbor != nullptr, "error");
	s_harborAddr.areaId = harbor->GetAttribute_S32("area_id");
	s_buffer = harbor->GetAttribute_S32("default_size");
	IXmlObject *node = harbor->GetFirstChrild("node");
	while (node)
	{
		NodeInfo nodeInfo;
		nodeInfo.name = node->GetAttribute_Str("name");
		nodeInfo.type = node->GetAttribute_S16("type");
		auto ret = s_nodes.insert(std::make_pair(nodeInfo.type, nodeInfo));
		ASSERT(ret.second, "error, type has insert, type = %d", nodeInfo.type);
		node = node->GetNextSibling();
	}
	node = harbor->GetFirstChrild("node");
	while (node)
	{
		s16 type = node->GetAttribute_S16("type");
		IXmlObject *buffdes = node->GetFirstChrild("buff");
		while (buffdes)
		{
			const char *name = buffdes->GetAttribute_Str("name");
			s32 recv = buffdes->GetAttribute_S32("recv");
			s32 send = buffdes->GetAttribute_S32("send");
			s16 target = GetNodeTypeByName(name);
			s32 key = GetBuffMapKey(type, target);
			auto ret = s_buffDes.insert(std::make_pair(key, GetBuffMapValue(send, recv)));
			ASSERT(ret.second, "error");
			buffdes = buffdes->GetNextSibling();
		}
		node = node->GetNextSibling();
	}

    return true;
}

s16 Harbor::GetNodeTypeByName(const char *name)
{
    for (auto iter = s_nodes.begin(); iter != s_nodes.end(); iter++)
    {
        if (iter->second.name == name)
            return iter->first;
    }
    ASSERT(false, "error");
    return -1;
}

const char * Harbor::GetNodeNameByNodeType(s8 nodeType)
{
	auto iter = s_nodes.find(nodeType);
	if (iter != s_nodes.end())
		return iter->second.name.GetString();

	return "NoneType";
}

void Harbor::OnRecvMasterConnectHarborMsg(s8 type, s8 nodeId, const char *buff, s32 len)
{
    ASSERT(sizeof(NODE_MSG_NODE_DISCOVER) == len, "error");
    NODE_MSG_NODE_DISCOVER *msg = (NODE_MSG_NODE_DISCOVER *)buff;
	sockaddr_in sockAddr;
	sockAddr.sin_addr.s_addr = msg->ipAddr;
    //struct in_addr addr;
    //addr.s_addr = msg->ipAddr;
    char ip[IP_LEN] = {0};
    //SafeSprintf(ip, sizeof(ip), "%s", inet_ntoa(addr));
	const char * tmpIp = tools::GetSocketIp(sockAddr);
	tools::SafeStrcpy(ip, sizeof(ip), tmpIp, strlen(tmpIp));

    s_self->ConnectHarbor(msg->nodeType, msg->nodeId, ip, msg->port);
}

void Harbor::OnRecvHarborHandshakeMsg(HarborSession *session, const char *buff, s32 len)
{
	s32 type = session->GetNodeType();
	s32 nodeId = session->GetNodeID();
	ASSERT(type == 0 && nodeId == 0, "error");

	if (type == 0 && nodeId == 0)
	{
		ASSERT(len == sizeof(NODE_MSG_HANDSHAKE), "error");
		NODE_MSG_HANDSHAKE *msg = (NODE_MSG_HANDSHAKE *)buff;
		session->SetNodeInfo(msg->nodeType, msg->nodeId);
		const char *ip = session->GetRemoteIP();
		IKernel *kernel = s_kernel;
		const char *name = GetNodeNameByNodeType(msg->nodeType);
		TRACE_LOG("Node :%s OnHandShake, type = %d, id = %d, ip = %s, port = %d", name, msg->nodeType, msg->nodeId, ip, msg->port);

		s64 channel = GetChannel(msg->nodeType, msg->nodeId);
		auto iter = s_sessions.find(channel);
		if (iter != s_sessions.end())
		{
			ERROR_LOG("NodeSession already exists, so need froce close.");
			session->Close("node session already exists");
			return;
		}

		s32 sendBuffSize = 0;
		s32 recvBuffSize = 0;
		auto buffIter = s_buffDes.find(GetBuffMapKey(s_nodeType, msg->nodeType));
		if (buffIter == s_buffDes.end())
		{
			sendBuffSize = s_buffer * BUFFER_SIZE_K;
			recvBuffSize = s_buffer * BUFFER_SIZE_K;
		}
		else
			GetSendAndRecvByValue(buffIter->second, sendBuffSize, recvBuffSize);

		auto ret = s_sessions.insert(std::make_pair(channel, session));
		ASSERT(ret.second, "error");
		if (!session->SettingBuffSize(recvBuffSize, sendBuffSize))
		{
			ERROR_LOG("node session setting buffsize error, recv buff size:%d, send buff size:%d", recvBuffSize, sendBuffSize);
			session->Close("setting buff size error");
			return;
		}
		
		session->SetIpInfo(ip, msg->port);

		DelayOpenTimer *delayOpenTimer = NEW DelayOpenTimer(session, s_self);
		s_kernel->StartTimer(delayOpenTimer, 0, 1, 100, "delay open timer");
		session->SetDelayTimer(delayOpenTimer);
	}
}