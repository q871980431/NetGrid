/*
 * File:	NetServer.cpp
 * Author:	xuping
 * 
 * Created On 2017/5/29 21:06:25
 */

#include "NetServer.h"
#include "Tools_time.h"
NetServer * NetServer::s_self = nullptr;
IKernel * NetServer::s_kernel = nullptr;
NetListener NetServer::s_netListener;
GameSession NetServer::s_session;


void GameSession::OnRecv(s32 messageId, const char *buff, s32 len)
{
	s64 now = tools::GetTimeMillisecond();
    _connection->Send(messageId, buff, len);
	if (_count == 0)
		_first = now;
	++_count;
	if (_count % 100000 == 0)
	{
		ECHO("%lld ms", now - _first);
		_first = now;
	}
}


bool NetServer::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool NetServer::Launched(IKernel *kernel)
{
    //s_kernel->CreateNetListener("0", 11401, &s_netListener);
    //s_kernel->CreateNetSession("192.168.124.1", 11401, &s_session);

    return true;
}

bool NetServer::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


