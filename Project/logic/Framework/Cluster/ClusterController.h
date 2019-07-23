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
class ClusterInfo;
class IEventEngine;
class ClusterConntroller
{
public:
	ClusterConntroller() {};

	bool Init(core::IKernel *kernel, bool master);
	bool InitClusterNode(s8 type, const char *name);
	bool InitCompelate();

	void OnClusterNodesTypeChange(IKernel *kernel, const void *context, s32 size);

public:
	bool AddClusterNode(s8 type, s8 nodeId);
	bool UpdateClusterNodeState(s8 type, s8 nodeId, s8 state);
	void DelClusterNode(s8 type, s8 nodeId);

private:
	void BrocastMsg(s32 messageId, const void *buff, s32 len);

private:
	core::IKernel		*_kernel;
	IHarbor				*_harbor;
	IEventEngine		*_eventEngine;

	std::bitset<0xFF>	_typebits;
	s8					_state;
	ClusterInfo			*_clusterInfo;
	bool				_master;
};
#endif