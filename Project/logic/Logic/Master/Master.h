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
#include <unordered_map>

enum NODE_TYPE
{
    NODE_TYPE_NONE = 0,
    NODE_TYPE_CONNECT = 1,
    NODE_TYPE_READY = 2,
    NODE_TYPE_RUNNING = 3,
};

struct  NodeInfo
{
    s8  state;
};

typedef std::unordered_map<s64, NodeInfo> NodeMap;
class Master : public IMaster, public INodeListener
{
public:
    virtual ~Master(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

    virtual void OnOpen(s32 type, s32 nodeId, const char *ip, s16 port);
    virtual void OnClose(s32 type, s32 nodeId);
protected:
private:
    static Master     * s_self;
    static IKernel    * s_kernel;
    static IHarbor    * s_harbor;
    static NodeMap      s_nodeMap;
};
#endif