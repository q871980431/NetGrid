/*
 * File:	RedisClient.h
 * Author:	xuping
 * 
 * Created On 2019/8/16 14:32:57
 */

#ifndef __RedisClient_h__
#define __RedisClient_h__
#include "IRedisClient.h"
class RedisClient : public IRedisClient
{
public:
    virtual ~RedisClient(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static RedisClient     * s_self;
    static IKernel  * s_kernel;
};
#endif