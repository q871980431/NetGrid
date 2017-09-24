/*
 * File:	Master.cpp
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:14:49
 */

#include "Master.h"
#include "Tools.h"
#include "FrameworkDefine.h"
Master * Master::s_self = nullptr;
IKernel * Master::s_kernel = nullptr;
IHarbor * Master::s_harbor = nullptr;
NodeMap   Master::s_nodeMap;

bool Master::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
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
    DEL this;
    return true;
}

void Master::OnOpen(s32 type, s32 nodeId, const char *ip, s16 port)
{
    NodeInfo nodeInfo;

    tools::KEYINT64 key;
    core::NODE_MSG_NODE_DISCOVER discover;
    discover.ipAddr = inet_addr(ip);
    discover.port = port;

    for (auto iter = s_nodeMap.begin(); iter != s_nodeMap.end(); iter++)
    {
        key.val = iter->first;
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
    nodeInfo.state = NODE_TYPE_CONNECT;
    auto ret = s_nodeMap.insert(std::make_pair(key.val, nodeInfo));
    ASSERT(ret.second, "error");
}

void Master::OnClose(s32 type, s32 nodeId)
{

}
