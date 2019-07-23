#include "ClusterInfo.h"
#include "IHarbor.h"
#include "IEventEngine.h"
#include "FrameworkDefine.h"

ClusterInfo::ClusterInfo(core::IKernel *kernel, bool master) :_kernel(kernel), _master(master)
{
	_state = core::CLUSTER_STATEUS_NONE;
	FIND_MODULE(_eventEngine, EventEngine);
}

bool ClusterInfo::InitClusterNode(s8 type, const char *name)
{
	if (GetClusterTypeNode(type) != nullptr)
	{
		ASSERT(false, "type repeated");
		return false;
	}
	ClusterTypeNodes *node = NEW ClusterTypeNodes(type, name);
	_clusterNodeMap.emplace(type, node);
	_typebits.set(type);
	return true;
}

bool ClusterInfo::InitCompelate()
{

	return true;
}

bool ClusterInfo::AddClusterNode(s8 type, s8 nodeId)
{
	ClusterTypeNodes *typeNode = GetClusterTypeNode(type);
	if (typeNode == nullptr)
		return false;
	return typeNode->AddNode(nodeId);
}

bool ClusterInfo::UpdateClusterNodeState(s8 type, s8 nodeId, s8 state)
{
	ClusterTypeNodes *typeNode = GetClusterTypeNode(type);
	if (typeNode == nullptr)
		return false;
	s8 oldState = typeNode->GetNodesState();
	bool ret = typeNode->UpdateNodeState(nodeId, state);
	if (ret)
	{
		s8 nodesState = typeNode->GetNodesState();
		if (oldState != nodesState)
		{
			if (_master && _eventEngine != nullptr)
			{
				core::ClusterNodeStatesChangeEvt evt;
				evt.type = type;
				evt.oldState = oldState;
				evt.nowState = nodesState;

				_eventEngine->ActiveEvent(core::FRAMEWORK_EVENT_ID::FE_CLUSTERNODE_STATE_CHANGE, &evt, sizeof(evt));
			}
		}
	}
	return true;
}

bool ClusterInfo::DelClusterNode(s8 type, s8 nodeId)
{
	ClusterTypeNodes *typeNode = GetClusterTypeNode(type);
	if (typeNode == nullptr)
	{
		ASSERT(false, "del error");
		return false;
	}
	s8 oldState = typeNode->GetNodesState();
	if (typeNode->DelNode(nodeId))
	{
		s8 nodeState = typeNode->GetNodesState();
		if (oldState != nodeState)
		{
			if (_master && _eventEngine != nullptr)
			{
				core::ClusterNodeStatesChangeEvt evt;
				evt.type = type;
				evt.oldState = oldState;
				evt.nowState = nodeState;

				_eventEngine->ActiveEvent(core::FRAMEWORK_EVENT_ID::FE_CLUSTERNODE_STATE_CHANGE, &evt, sizeof(evt));
			}
		}

		return true;
	}

	return false;
}

ClusterTypeNodes * ClusterInfo::GetClusterTypeNode(s8 type)
{
	auto iter = _clusterNodeMap.find(type);
	if (iter == _clusterNodeMap.end())
		return nullptr;
	return iter->second;
}

void ClusterInfo::OnClusterNodeTypeChange(ClusterTypeNodes *typeNodes, s8 oldState, s8 nowState)
{
	if (oldState == core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_NONE && nowState == core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_RUNNING)
	{

		if (!_typebits.test(typeNodes->GetNodesType()))
		{
			ASSERT(false, "error, has unlock");
			return;
		}
		_typebits.reset(typeNodes->GetNodesType());
		if (_typebits.none())
		{
			ASSERT(_state == core::CLUSTER_STATEUS_NONE, "error");

			//_harbor->SendMessage();
			_state = core::CLUSTER_STATEUS_RUNNING;
		}
	}

}

void ClusterInfo::BrocastMsg(s32 messageId, const void *buff, s32 len)
{
	for (auto &iter : _clusterNodeMap)
	{
		const auto &nodemap = iter.second->GetNodeMap();
		for (const auto &nodeIter : nodemap)
			_harbor->SendMessage(iter.first, nodeIter.second.nodeId, messageId, buff, len);
	}
}






bool ClusterTypeNodes::AddNode(s8 nodeId)
{
	auto iter = _nodeMap.find(nodeId);
	if (iter != _nodeMap.end())
	{
		ASSERT(false, "error");
		return false;
	}
	NodeInfo node;
	node.nodeId = nodeId;
	node.state = core::NODE_STATUS_CONNECT;
	_nodeMap.emplace(nodeId, node);
	return true;
}

bool ClusterTypeNodes::UpdateNodeState(s8 nodeId, s8 state)
{
	auto iter = _nodeMap.find(nodeId);
	if (iter == _nodeMap.end())
	{
		ASSERT(false, "error");
		return false;
	}
	iter->second.state = state;
	CheckTypeNodeState();
	return true;
}

bool ClusterTypeNodes::DelNode(s8 nodeId)
{
	auto iter = _nodeMap.find(nodeId);
	if (iter != _nodeMap.end())
	{
		_nodeMap.erase(iter);
		CheckTypeNodeState();
		return true;
	}

	return false;
}

void ClusterTypeNodes::CheckTypeNodeState()
{
	//State Change
	if (_state == core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_NONE)
	{
		bool mark = false;
		for (const auto &node:_nodeMap)
		{
			if (node.second.state == core::NODE_STATUS::NODE_STATUS_READY)
			{
				mark = true;
				break;
			}
		}
		if (mark)
		{
			_state = core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_RUNNING;
			return;
		}
	}

	if (_state == core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_RUNNING)
	{
		bool mark = false;
		for (const auto &node : _nodeMap)
		{
			if (node.second.state != core::NODE_STATUS::NODE_STATUS_NONE)
			{
				mark = true;
				break;
			}
		}
		if (!mark)
		{
			_state = core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_NONE;
			return;
		}
	}
}
