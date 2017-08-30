/*
 * File:	CacheDB.h
 * Author:	xuping
 * 
 * Created On 2017/5/16 20:06:11
 */

#ifndef __CacheDB_h__
#define __CacheDB_h__
#include "ICacheDB.h"
class ITest;
class IRedis;
class CacheDB : public ICacheDB
{
public:
    virtual ~CacheDB(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static CacheDB     * s_self;
    static IKernel     * s_kernel;

    static ITest        * s_test;
    static IRedis       * s_redis;
};
#endif