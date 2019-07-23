/*
 * File:	Cluster.h
 * Author:	xuping
 * 
 * Created On 2019/6/17 20:00:12
 */

#ifndef __Cluster_h__
#define __Cluster_h__
#include "ICluster.h"
#include "ClusterController.h"
#include "IHarbor.h"

#include <unordered_map>

struct ClusterNodeInfo
{
	s8 nodeType;
	s8 nodeId;
	std::string ip;
	s32 port;
};

typedef std::unordered_map<s64, ClusterNodeInfo> ClusterNodeInfoMap;

class Cluster : public ICluster, public INodeListener
{
public:
    virtual ~Cluster(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

	virtual void OnOpen(s8 type, s8 nodeId, const char *ip, s32 port);
	virtual void OnClose(s8 type, s8 nodeId);

private:
	static void OnRecvClusterAdhocMsg(s8 type, s8 nodeId, const char *buff, s32 len);

private:
	static void OnClusterNodesTypeChange(IKernel *kernel, const void *context, s32 size);

private:
	bool LoadClusterConfig(IKernel *kernel);
	s64  GetSessionChannel(s8 type, s8 nodeId);

private:
    static Cluster		* s_self;
    static IKernel		* s_kernel;
	static IHarbor		* s_harbor;

	static ClusterConntroller * s_clusterConroller;
	static std::string			s_masterIp;
	static s32					s_masterPort;
	static ClusterNodeInfoMap		s_clusterNodeMap;
};
#endif