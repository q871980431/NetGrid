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
#ifdef LINUX
#include <arpa/inet.h>
#endif

Harbor * Harbor::s_self = nullptr;
IKernel * Harbor::s_kernel = nullptr;
NodeListeners Harbor::s_listeners;
MsgCallBackMap Harbor::s_callbacks;
s32         Harbor::s_nodeType;
s32         Harbor::s_nodeId;
s32         Harbor::s_buffer;
s16         Harbor::s_port;
MasterInfo  Harbor::s_master;
NodeMap     Harbor::s_nodes;
BufferDesMap Harbor::s_buffDes;
SessionMap  Harbor::s_sessions;



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
    s_nodeType = StrToInt32(nodeType);
    s_nodeId = StrToInt32(nodeId);
    s_port = StrToInt32(port);
    const char *corePath = s_kernel->GetCoreFile();
    LoadConfigFile(corePath);


    return true;
}

bool Harbor::Launched(IKernel *kernel)
{
    s_kernel->CreateNetListener("0", s_port, this);
    if (s_nodeType != core::NODE_TYPE_MASTER)
        ConnectHarbor(s_master.ip.GetString(), s_master.port);
    RegisterMessage(core::NODE_MSG_CONNECT_HARBOR, OnRecvMasterConnectHarborMsg);

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

void Harbor::RegisterMessage(s32 messageId, CALL_BACK fun)
{
    auto ret = s_callbacks.insert(std::make_pair(messageId, fun));
    ASSERT(ret.second, "insert error");
}

void Harbor::Sendbuff(s32 type, s32 nodeId, const void *buff, s32 len)
{
    s64 key = GetChannel(type, nodeId);
    auto iter = s_sessions.find(key);
    if (iter != s_sessions.end())
        iter->second->SendBuff((const char *)buff, len);
}

void Harbor::SendMessage(s32 type, s32 nodeId, s32 messageId, const void *buff, s32 len)
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

HarborClientSession::~HarborClientSession()
{
    if (_timer != nullptr)
    {
        _kernel->KillTimer(_timer);
    }
}

void HarborClientSession::OnTerminate()
{
    _harbor->OnClose(this);
    StartTimer();
}

void HarborClientSession::OnError(s32 moduleErr, s32 sysErr)
{
    StartTimer();
}

void HarborClientSession::StartTimer()
{
    _connectCount++;
    if (_connectCount <= 10)
    {
        ASSERT(_timer == nullptr, "error");
        _timer = NEW ReConnectTimer(this);
        _kernel->StartTimer(_timer, 0, 1, (_connectCount << 1) - 1, "ReConnect Timer");
    }
    else
    {
        DEL this;
    }
}

void Harbor::OnOpen(HarborSession *session)
{
    NODE_MSG_HANDSHAKE msg;
    msg.nodeType = s_nodeType;
    msg.nodeId = s_nodeId;
    msg.port = s_port;
    core::MessageHead head;
    head.messageId = core::NODE_MSG_LOGIN_MASTER;
    head.len = sizeof(msg) + sizeof(head);

    session->SendBuff((const char *)&head, sizeof(head));
    session->SendBuff((const char *)&msg, sizeof(msg));
}

void Harbor::OnRecv(HarborSession *session, s32 messageId, const char *buff, s32 len)
{
    s32 type = session->GetType();
    s32 nodeId = session->GetID();

    if (messageId == core::NODE_MSG_LOGIN_MASTER)
    {
        ASSERT(type == 0 && nodeId == 0, "error");
        if (type == 0 && nodeId == 0)
        {
            ASSERT(len == sizeof(NODE_MSG_HANDSHAKE), "error");
            NODE_MSG_HANDSHAKE *msg = (NODE_MSG_HANDSHAKE *)buff;
            session->SetNodeInfo(msg->nodeType, msg->nodeId);
            const char *ip = session->GetRemoteIP();
            auto endIter = s_listeners.end();
			IKernel *kernel = s_kernel;
            TRACE_LOG("Node OnOpen, type = %d, id = %d, ip = %s, port = %d", msg->nodeType, msg->nodeId, ip, msg->port);
			ECHO("Node OnOpen, type = %d, id = %d, ip = %s, port = %d", msg->nodeType, msg->nodeId, ip, msg->port);


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

            auto ret = s_sessions.insert(std::make_pair(GetChannel(msg->nodeType, msg->nodeId), session));
            ASSERT(ret.second, "error");
            session->SettingBuffSize(recvBuffSize, sendBuffSize);
            for (auto iter = s_listeners.begin(); iter != endIter; iter++)
                (*iter)->OnOpen(msg->nodeType, msg->nodeId, ip, msg->port);
        }
    }
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
    s32 type = session->GetType();
    s32 nodeId = session->GetID();
    if ( type != 0 || nodeId != 0)
    {
        for (auto iter = s_listeners.begin(); iter != endIter; iter++)
            (*iter)->OnClose(type, nodeId);
    }

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
    IXmlObject *master = root->GetFirstChrild("master");
    ASSERT(master != nullptr, "error");
    s_master.ip = master->GetAttribute_Str("ip");
    s_master.port = master->GetAttribute_S16("port");
    IXmlObject *harbor = root->GetFirstChrild("harbor");
    ASSERT(harbor != nullptr, "error");
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

void Harbor::OnRecvMasterConnectHarborMsg(s32 type, s32 nodeId, const char *buff, s32 len)
{
    ASSERT(sizeof(NODE_MSG_NODE_DISCOVER) == len, "error");
    NODE_MSG_NODE_DISCOVER *msg = (NODE_MSG_NODE_DISCOVER *)buff;
    struct in_addr addr;
    addr.s_addr = msg->ipAddr;
    char ip[IP_LEN] = {0};
    SafeSprintf(ip, sizeof(ip), "%s", inet_ntoa(addr));
    ConnectHarbor(ip, msg->port);
}

void Harbor::ConnectHarbor(const char *ip, s16 port)
{
    HarborClientSession *session = NEW HarborClientSession(s_kernel, s_self, ip, port);
    s_kernel->CreateNetSession(ip, port, session);
}