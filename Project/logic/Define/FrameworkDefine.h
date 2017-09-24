#ifndef __FrameworkDefine_h__
#define __FrameworkDefine_h__
#include "MultiSys.h"
namespace core
{
    const static s32 NODE_MSG_MAX = 256;
    const static s32 NODE_NAME_LEN = 32;
    enum NODE_MSG_ID
    {
        NODE_MSG_LOGIN_MASTER   =  1,
        NODE_MSG_CONNECT_HARBOR = 2,
    };
    enum NODE_TYPE
    {
        NODE_TYPE_SLAVE = -1,
        NODE_TYPE_MASTER = 0,
    };

    struct NODE_MSG_HANDSHAKE
    {
        s32 nodeId;
        s32 nodeType;
        s16 port;
    };

    struct NODE_MSG_PING 
    {
        s64 tick;
    };

    struct NODE_MSG_NODE_DISCOVER
    {
        s32 ipAddr;
        s16 port;
    };

}

#endif