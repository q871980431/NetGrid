/*
 * File:	Harbor.cpp
 * Author:	XuPing
 * 
 * Created On 2017/9/3 22:51:33
 */

#include "Harbor.h"
#include "FrameworkDefine.h"
#include "XmlReader.h"

Harbor * Harbor::s_self = nullptr;
IKernel * Harbor::s_kernel = nullptr;
NodeListeners Harbor::s_listeners;
MsgCallBackMap Harbor::s_callbacks;
s32         Harbor::s_nodeType;
s32         Harbor::s_nodeId;
s16         Harbor::s_port;
MasterInfo  Harbor::s_master;
NodeMap     Harbor::s_nodes;



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
   // s_nodeType = s_kernel->GetCmdArg()

    return true;
}

bool Harbor::Launched(IKernel *kernel)
{

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

void Harbor::Sendbuff(s32 type, s32 nodeId, const char *buff, s32 len)
{

}

void Harbor::SendMessage(s32 type, s32 nodeId, s32 messageId, const char *buff, s32 len)
{
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
    session->SendBuff((const char *)&msg, sizeof(head));
}

void Harbor::OnRecv(HarborSession *session, s32 messageId, const char *buff, s32 len)
{
    if (messageId == core::NODE_MSG_LOGIN_MASTER)
    {
        s32 type = session->GetType();
        s32 nodeId = session->GetID();
        ASSERT(type == 0 && nodeId == 0, "error");
        if (type == 0 && nodeId == 0)
        {
            ASSERT(len == sizeof(NODE_MSG_HANDSHAKE), "error");
            NODE_MSG_HANDSHAKE *msg = (NODE_MSG_HANDSHAKE *)buff;
            session->SetNodeInfo(msg->nodeType, msg->nodeId);
            const char *ip = session->GetRemoteIP();
            auto endIter = s_listeners.end();
            DEBUG_LOG("Node OnOpen, type = %d, id = %d, ip = %s, port = %d", msg->nodeType, msg->nodeId, ip, msg->port);
            for (auto iter = s_listeners.begin(); iter != endIter; iter++)
                (*iter)->OnOpen(msg->nodeType, msg->nodeId, ip, msg->port);
        }
    }
    else
    {
        auto iter = s_callbacks.find(messageId);
        if (iter != s_callbacks.end())
            iter->second(buff, len);
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
        IXmlObject *buffdes = node->GetFirstChrild("buff");
        while (buffdes)
        {
            ;
        }
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

    return -1;
}