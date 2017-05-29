/*
 * File:	NetServer.cpp
 * Author:	xuping
 * 
 * Created On 2017/5/29 21:06:25
 */

#include "NetServer.h"
NetServer * NetServer::s_self = nullptr;
IKernel * NetServer::s_kernel = nullptr;
bool NetServer::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool NetServer::Launched(IKernel *kernel)
{

    return true;
}

bool NetServer::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


