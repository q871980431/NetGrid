/*
 * File:	NetServer.h
 * Author:	xuping
 * 
 * Created On 2017/5/29 21:06:25
 */

#ifndef __NetServer_h__
#define __NetServer_h__
#include "INetServer.h"
class NetServer : public INetServer
{
public:
    virtual ~NetServer(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static NetServer     * s_self;
    static IKernel  * s_kernel;
};
#endif