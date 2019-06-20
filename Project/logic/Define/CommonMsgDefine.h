#ifndef __CommonMsgDefine_h__
#define __CommonMsgDefine_h__
#include "MultiSys.h"
namespace core
{
	const static s32 NODE_MSG_ID_MIN = 16 * 1024;
	const static s32 NODE_MSG_ID_MAX = 64 * 1024;
	enum COMMON_MSG
	{
		NODE_MSG_LOGIN_MASTER = 1,
		NODE_MSG_CONNECT_HARBOR = 2,
		NODE_MSG_NODE_STATUS_UPDATE = 3,
		NODE_MSG_CLUSTER_STATE_CHANGE = 4,
	};
}
#endif
