#ifndef __FrameworkDefine_h__
#define __FrameworkDefine_h__
#include "MultiSys.h"
namespace core
{
    const static s32 NODE_MSG_MAX = 256;
    const static s32 NODE_NAME_LEN = 32;
	const static s32 EVENT_LOCK_NUM_MAX = 128;
	const static s32 NODE_CMD_LEN = 256;
	const static s16 NODE_TYPE_MAX = 4096;
	const static s32 INVALID = -1;

    enum NODE_MSG_ID
    {
    };

    enum NODE_TYPE
    {
        NODE_TYPE_SLAVE = -1,
        NODE_TYPE_MASTER = 0,
		NODE_TYPE_GATE = 1,
		NODE_TYPE_LOGIC = 2,
		NODE_TYPE_RELATION = 3,
		NODE_TYPE_DB = 4,
    };

	enum CLUSTER_STATEUS
	{
		CLUSTER_STATEUS_NONE = 0,
		CLUSTER_STATEUS_RUNNING = 1,
	};

	enum TYPE_SERVER_STATUS
	{
		TYPE_SERVER_STATUS_NONE = 0,
		TYPE_SERVER_STATUS_RUNNING = 1,
	};

	enum NODE_STATUS
	{
		NODE_STATUS_NONE = 0,
		NODE_STATUS_CONNECT = 1,
		NODE_STATUS_READY = 2,
		NODE_STATUS_RUNNING = 3,
		NODE_STATUS_BUSY = 4,
		NODE_STATUS_REFUSE = 5,
	};

	enum FRAMEWORK_EVENT_ID
	{
		FRAMEWORK_EVENT_ID_MIN = 0,
		FRAMEWORK_EVENT_CREATE_EVENTLOCK = 1,

		FRAMEWORK_EVENT_ID_MAX,
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

	struct NODE_STATUS_UPDATE 
	{
		s32 status;
	};

	struct NODE_MSG_SATRT_SERVICE
	{
		char name[NODE_NAME_LEN];
		s8 type;
		s32 id;
		char cmd[NODE_CMD_LEN];
	};

	namespace event {
		struct CreateEventLock {
			s32 eventId;
		};
	}

}

#endif