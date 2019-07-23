#ifndef __CommonMsgDefine_h__
#define __CommonMsgDefine_h__
#include "MultiSys.h"
namespace core
{
	const static s32 NODE_MSG_ID_MIN = 16 * 1024;
	const static s32 NODE_MSG_ID_MAX = 64 * 1024;
	enum COMMON_MSG_ID
	{
		NODE_MSG_ID_HAND_SHAKE = 1,
		NODE_MSG_ID_CLUSTER_ADHOC = 2,
		NODE_MSG_NODE_STATUS_UPDATE = 3,
		NODE_MSG_CLUSTER_STATE_CHANGE = 4,
	};

	struct NODE_MSG_NODE_DISCOVER
	{
		s8  nodeType;
		s8  nodeId;
		s32 ipAddr;
		s16 port;
	};

	typedef NODE_MSG_NODE_DISCOVER NODE_MSG_CLUSTER_ADHOC_NETWORK;
}
#endif
