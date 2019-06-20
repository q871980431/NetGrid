/*
 * File:	NetService.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#include "NetService.h"
#include "../Kernel.h"
#include "NetListenerMgr.h"
#include "TcpConnection.h"
#include "Tools_time.h"
#ifdef WIN32
#include <winsock2.h>
#endif

NetService * NetService::s_self = nullptr;
IKernel * NetService::s_kernel = nullptr;
bool NetService::Ready()
{
	IKernel *kernel = &Kernel::GetInstance();
	s_self = this;
	s_kernel = kernel;
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	(void)WSAStartup(wVersionRequested, &wsaData);
#endif
	_listenerMgr = NEW NetListenerMgr(this, kernel);
	_netIoEngine = NEW NetIOEngine(4);
	_netIoEngine->Init(kernel);

	return true;
}

bool NetService::Initialize()
{
    return true;
}

bool NetService::Destroy()
{
	DEL _listenerMgr;
	DEL _netIoEngine;
    DEL this;
    return true;
}

void NetService::Process(core::IKernel *kernel, s32 exTick)
{
	s64 tick = tools::GetTimeMillisecond();
	_listenerMgr->Process(kernel, tick);
	_netIoEngine->Process(kernel, tick);
	for (auto &iter : _connectionMap)
	{
		iter.second->OnRecv(kernel);
	}
}

void NetService::CreateNetListener(const char *ip, s32 port, INetTcpListener *listener)
{
	_listenerMgr->CreateNetListener(s_kernel, ip, port, listener);
}

void NetService::CreateNetSession(const char *ip, s32 port, ITcpSession *session)
{
	_listenerMgr->CreateNetConnecter(s_kernel, ip, port, session);
}

void NetService::OnAcceptSocket(IKernel *kernel, NetSocket clientSocket, ITcpSession *session)
{
	TRACE_LOG("On Accept Socket, Socket:%d", clientSocket);
	CreateConnection(GetSessionId(), true, clientSocket, session);
}

void NetService::OnConnectedSocket(IKernel *kernel, NetSocket connectSocket, sockaddr_in &addr, ITcpSession *session)
{
	TRACE_LOG("On Connect Target Ip£º%s, Port:%d, Socket:%d", tools::GetSocketIp(addr), tools::GetSocketPort(addr), connectSocket);
	CreateConnection(GetSessionId(), false, connectSocket, session);
}

TcpConnection *  NetService::CreateConnection(s32 sessionId, bool passive, NetSocket netSocket, ITcpSession *session)
{
	auto iter = _connectionMap.find(sessionId);
	ASSERT(iter == _connectionMap.end(), "error");
	if (iter == _connectionMap.end())
	{
		TcpConnection *connection = NEW TcpConnection(sessionId, passive, netSocket, session, this);
		connection->Init();
		_netIoEngine->AddConnection(connection);
		_connectionMap.emplace(sessionId, connection);
		return connection;
	}

	return nullptr;
}

void NetService::RemoveConnection(s32 sessionId, bool recvFin)
{
	IKernel *kernel = s_kernel;
	TRACE_LOG("Enter Remove connection:seesionId:%d, recvFin:%d", sessionId, recvFin);
	auto iter = _connectionMap.find(sessionId);
	ASSERT(iter != _connectionMap.end(), "error");
	if (iter != _connectionMap.end())
	{
		TRACE_LOG("Remove connection:seesionId:%d, recvFin:%d sucess", sessionId, recvFin);
		NetSocket netSocket = iter->second->GetNetSocket();
		if (recvFin)
			tools::CloseSocket(netSocket);
		else
			tools::SocketFroceClose(netSocket);

		_netIoEngine->RemoveConnection(iter->second);
		DEL iter->second;
		_connectionMap.erase(iter);
	}
}
