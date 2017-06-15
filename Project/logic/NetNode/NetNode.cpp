/*
 * File:	NetNode.cpp
 * Author:	xuping
 * 
 * Created On 2017/6/13 0:05:47
 */

#include "NetNode.h"
NetNode * NetNode::s_self = nullptr;
IKernel * NetNode::s_kernel = nullptr;
bool NetNode::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool NetNode::Launched(IKernel *kernel)
{

    return true;
}

bool NetNode::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


