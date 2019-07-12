#include "ClusterController.h"
#include "IHarbor.h"
ClusterConntroller::ClusterConntroller(core::IKernel *kernel, IHarbor *harbor) :_kernel(kernel), _harbor(harbor)
{
	_state = core::CLUSTER_STATEUS_NONE;
}

bool ClusterConntroller::InitClusterNode(s8 type, const char *name)
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

bool ClusterConntroller::InitCompelate()
{

	return true;
}

bool ClusterConntroller::AddClusterNode(s8 type, s8 nodeId)
{
	ClusterTypeNodes *typeNode = GetClusterTypeNode(type);
	if (typeNode == nullptr)
		return false;
	return typeNode->AddNode(nodeId);
}

bool ClusterConntroller::UpdateClusterNodeState(s8 type, s8 nodeId, s8 state)
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
			OnClusterNodeTypeChange(typeNode, oldState, nodesState);
	}
	return true;
}

ClusterTypeNodes * ClusterConntroller::GetClusterTypeNode(s8 type)
{
	auto iter = _clusterNodeMap.find(type);
	if (iter == _clusterNodeMap.end())
		return nullptr;
	return iter->second;
}

void ClusterConntroller::OnClusterNodeTypeChange(ClusterTypeNodes *typeNodes, s8 oldState, s8 nowState)
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

void ClusterConntroller::BrocastMsg(s32 messageId, const void *buff, s32 len)
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
