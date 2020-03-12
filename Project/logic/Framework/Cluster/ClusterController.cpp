#include "ClusterController.h"
#include "ClusterInfo.h"
#include "IHarbor.h"
#include "IEventEngine.h"
#include "FrameworkDefine.h"

bool ClusterConntroller::Init(core::IKernel *kernel, bool master)
{
	_kernel = kernel;
	_master = master;
	_state = core::CLUSTER_STATEUS_NONE;
	_clusterInfo = NEW ClusterInfo(kernel, _master);
	FIND_MODULE(_eventEngine, EventEngine);
	if (_eventEngine == nullptr)
		return false;

	return true;
}

bool ClusterConntroller::InitClusterNode(s8 type, const char *name)
{
	if (_master)
	{
		if (_clusterInfo->InitClusterNode(type, name))
		{
			_typebits.set(type);
			return true;
		}
	}

	return true;
}

bool ClusterConntroller::InitCompelate()
{
	if (_eventEngine == nullptr)
		return false;

	if (_master)
	{
		if (!_clusterInfo->InitCompelate())
			return false;
	}
	//REG_EVENT_LISTENER(_eventEngine, core::FRAMEWORK_EVENT_ID::FE_CLUSTERNODE_STATE_CHANGE, OnClusterNodesTypeChange);

	return true;
}

void ClusterConntroller::OnClusterNodesTypeChange(IKernel *kernel, const void *context, s32 size)
{
	const core::ClusterNodeStatesChangeEvt *evt = CAST_DATA_PTR(core::ClusterNodeStatesChangeEvt, context, size);
	if (evt == nullptr)
	{
		ASSERT(false, "error");
		return;
	}

	if (evt->oldState == core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_NONE && evt->nowState == core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_RUNNING)
	{
	
		if (!_typebits.test(evt->type))
		{
			ASSERT(false, "error, has unlock");
			return;
		}
		_typebits.reset(evt->type);
		if (_typebits.none())
		{
			ASSERT(_state == core::CLUSTER_STATEUS_NONE, "error");
	
			//_harbor->SendMessage();
			_state = core::CLUSTER_STATEUS_RUNNING;
		}
	}

}

bool ClusterConntroller::AddClusterNode(s8 type, s8 nodeId)
{
	_clusterInfo->AddClusterNode(type, nodeId);
	return true;
	//ClusterTypeNodes *typeNode = GetClusterTypeNode(type);
	//if (typeNode == nullptr)
	//	return false;
	//return typeNode->AddNode(nodeId);
}

bool ClusterConntroller::UpdateClusterNodeState(s8 type, s8 nodeId, s8 state)
{
	return _clusterInfo->UpdateClusterNodeState(type, nodeId, state);
}

void ClusterConntroller::DelClusterNode(s8 type, s8 nodeId)
{
	_clusterInfo->DelClusterNode(type, nodeId);
}

//void ClusterConntroller::OnClusterNodeTypeChange(ClusterTypeNodes *typeNodes, s8 oldState, s8 nowState)
//{
//	if (oldState == core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_NONE && nowState == core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_RUNNING)
//	{
//
//		if (!_typebits.test(typeNodes->GetNodesType()))
//		{
//			ASSERT(false, "error, has unlock");
//			return;
//		}
//		_typebits.reset(typeNodes->GetNodesType());
//		if (_typebits.none())
//		{
//			ASSERT(_state == core::CLUSTER_STATEUS_NONE, "error");
//
//			//_harbor->SendMessage();
//			_state = core::CLUSTER_STATEUS_RUNNING;
//		}
//	}
//}

void ClusterConntroller::BrocastMsg(s32 messageId, const void *buff, s32 len)
{

}
