/*
 * File:	Redis.h
 * Author:	xuping
 * 
 * Created On 2017/5/16 19:55:32
 */

#ifndef __Redis_h__
#define __Redis_h__
#include "IRedis.h"
class Redis : public IRedis
{
public:
    virtual ~Redis(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static Redis     * s_self;
    static IKernel  * s_kernel;
};
#endif