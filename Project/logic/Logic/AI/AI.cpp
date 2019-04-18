/*
 * File:	AI.cpp
 * Author:	xuping
 * 
 * Created On 2019/3/22 11:59:34
 */

#include "AI.h"
AI * AI::s_self = nullptr;
IKernel * AI::s_kernel = nullptr;
bool AI::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool AI::Launched(IKernel *kernel)
{

    return true;
}

bool AI::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


