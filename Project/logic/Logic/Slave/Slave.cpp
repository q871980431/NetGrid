/*
 * File:	Slave.cpp
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:15:12
 */

#include "Slave.h"
Slave * Slave::s_self = nullptr;
IKernel * Slave::s_kernel = nullptr;
bool Slave::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool Slave::Launched(IKernel *kernel)
{

    return true;
}

bool Slave::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


