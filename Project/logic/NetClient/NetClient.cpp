/*
 * File:	NetClient.cpp
 * Author:	xuping
 * 
 * Created On 2017/5/29 21:05:47
 */

#include "NetClient.h"
NetClient * NetClient::s_self = nullptr;
IKernel * NetClient::s_kernel = nullptr;
bool NetClient::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool NetClient::Launched(IKernel *kernel)
{

    return true;
}

bool NetClient::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


