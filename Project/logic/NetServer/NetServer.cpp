/*
 * File:	NetServer.cpp
 * Author:	xuping
 * 
 * Created On 2017/5/29 21:06:25
 */

#include "NetServer.h"
NetServer * NetServer::s_self = nullptr;
IKernel * NetServer::s_kernel = nullptr;
NetListener NetServer::s_netListener;
GameSession NetServer::s_session;


void GameSession::OnRecv(s32 messageId, const char *buff, s32 len)
{
    if (messageId == 1)
        ECHO("%s", buff);
    _connection->Send(messageId, buff, len);
}


bool NetServer::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool NetServer::Launched(IKernel *kernel)
{
    s_kernel->CreateNetListener("0", 11401, &s_netListener);
    s_kernel->CreateNetSession("127.0.0.1", 11401, &s_session);

    return true;
}

bool NetServer::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


