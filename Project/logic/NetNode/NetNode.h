/*
 * File:	NetNode.h
 * Author:	xuping
 * 
 * Created On 2017/6/13 0:05:47
 */

#ifndef __NetNode_h__
#define __NetNode_h__
#include "INetNode.h"
class NetNode : public INetNode
{
public:
    virtual ~NetNode(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static NetNode     * s_self;
    static IKernel  * s_kernel;
};
#endif