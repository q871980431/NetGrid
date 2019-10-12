/*
 * File:	RedisClient.cpp
 * Author:	xuping
 * 
 * Created On 2019/8/16 14:32:57
 */

#include "RedisClient.h"
RedisClient * RedisClient::s_self = nullptr;
IKernel * RedisClient::s_kernel = nullptr;
bool RedisClient::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool RedisClient::Launched(IKernel *kernel)
{

    return true;
}

bool RedisClient::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


