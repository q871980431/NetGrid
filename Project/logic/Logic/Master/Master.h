/*
 * File:	Master.h
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:14:49
 */

#ifndef __Master_h__
#define __Master_h__
#include "IMaster.h"
#include "IHarbor.h"
#include "TString.h"
#include "FrameworkDefine.h"
#include <unordered_map>
#include <map>
#include <set>

struct SlaveInfo 
{
	s8 state;

};
struct  NodeInfo
{
	s32 nodeId;
	s8  state;
};


typedef std::set<s64> NodeMap;
typedef std::map<s32, NodeInfo> SlaveMap;
class ServiceGroup;
class Master : public IMaster, public INodeListener
{
public:
    virtual ~Master(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

    virtual void OnOpen(s32 type, s32 nodeId, const char *ip, s16 port);
    virtual void OnClose(s32 type, s32 nodeId);

private:
	void OnRecvNodeHasReadyMsg(s32 type, s32 nodeId, const char *buff, s32 len);

private:
	bool LoadConfigFile(const char *path);
protected:
private:
    static Master     * s_self;
    static IKernel    * s_kernel;
    static IHarbor    * s_harbor;
	static ServiceGroup *s_serviceGroup;

    static NodeMap      s_nodeMap;
	static SlaveMap		s_slaves;

};
#endif