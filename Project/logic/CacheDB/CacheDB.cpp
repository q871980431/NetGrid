/*
 * File:	CacheDB.cpp
 * Author:	xuping
 * 
 * Created On 2017/5/16 20:06:11
 */

#include "CacheDB.h"
#include "ITest.h"
#include "IRedis.h"
CacheDB * CacheDB::s_self = nullptr;
IKernel * CacheDB::s_kernel = nullptr;
ITest   * CacheDB::s_test = nullptr;
IRedis  * CacheDB::s_redis = nullptr;
bool CacheDB::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool CacheDB::Launched(IKernel *kernel)
{
    FIND_MODULE(s_test, Test);
    FIND_MODULE(s_redis, Redis);
    s32 add = s_test->Add(2, 3);
    ECHO("num = %d", add);
    return true;
}

bool CacheDB::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


