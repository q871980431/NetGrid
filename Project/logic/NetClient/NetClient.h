/*
 * File:	NetClient.h
 * Author:	xuping
 * 
 * Created On 2017/5/29 21:05:47
 */

#ifndef __NetClient_h__
#define __NetClient_h__
#include "INetClient.h"
class NetClient : public INetClient
{
public:
    virtual ~NetClient(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static NetClient     * s_self;
    static IKernel  * s_kernel;
};
#endif