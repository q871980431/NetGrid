/*
 * File:	Master.cpp
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:14:49
 */

#include "Master.h"
Master * Master::s_self = nullptr;
IKernel * Master::s_kernel = nullptr;
bool Master::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool Master::Launched(IKernel *kernel)
{

    return true;
}

bool Master::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


