/*
 * File:	Redis.cpp
 * Author:	xuping
 * 
 * Created On 2017/5/16 19:55:32
 */

#include "Redis.h"
Redis * Redis::s_self = nullptr;
IKernel * Redis::s_kernel = nullptr;
bool Redis::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool Redis::Launched(IKernel *kernel)
{
    return true;
}

bool Redis::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


