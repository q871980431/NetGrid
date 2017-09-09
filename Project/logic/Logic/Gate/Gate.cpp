/*
 * File:	Gate.cpp
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:15:22
 */

#include "Gate.h"
Gate * Gate::s_self = nullptr;
IKernel * Gate::s_kernel = nullptr;
bool Gate::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool Gate::Launched(IKernel *kernel)
{

    return true;
}

bool Gate::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


