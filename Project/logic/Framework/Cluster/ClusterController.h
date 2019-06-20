/*
 * File:	Cluster.h
 * Author:	xuping
 * 
 * Created On 2019/6/17 20:00:12
 */

#ifndef __ClusterController_h__
#define __ClusterController_h__
#include "MultiSys.h"
#include "ICluster.h"
#include "FrameworkDefine.h"
#include <map>
#include <bitset>
#include "IHarbor.h"

class ClusterTypeNodes;
typedef std::map<s8, ClusterTypeNodes *> ClusterNodeMap;
class ClusterConntroller
{
public:
	ClusterConntroller(core::IKernel *kernel, IHarbor *harbor);

	bool InitClusterNode(s8 type, const char *name);
	bool InitCompelate();
	bool AddClusterNode(s8 type, s8 nodeId);
	bool UpdateClusterNodeState(s8 type, s8 nodeId, s8 state);
protected:

private:
	ClusterTypeNodes * GetClusterTypeNode(s8 type);
	void OnClusterNodeTypeChange(ClusterTypeNodes *typeNodes, s8 oldState, s8 nowState);
	void BrocastMsg(s32 messageId, const void *buff, s32 len);
private:
	core::IKernel		*_kernel;
	IHarbor				*_harbor;
	ClusterNodeMap		_clusterNodeMap;
	std::bitset<0xFF>	_typebits;
	s8					_state;
};

struct NodeInfo 
{
	s8 nodeId;
	s8 state;
};

typedef std::map<s8, NodeInfo> NodeMap;
class ClusterTypeNodes
{
public:
	ClusterTypeNodes(s8 type, const char *name) :_type(type), _name(name) {
		_state = core::TYPE_SERVER_STATUS::TYPE_SERVER_STATUS_NONE;
	};

	bool AddNode(s8 nodeId);
	bool UpdateNodeState(s8 nodeId, s8 state);
	inline s8 GetNodesState() { return _state; };
	inline s8 GetNodesType() { return _type; };
	inline const NodeMap & GetNodeMap() { return _nodeMap; };
protected:
	void CheckTypeNodeState();
private:
	s8 _type;
	std::string _name;
	s8 _state;
	NodeMap _nodeMap;
};
#endif