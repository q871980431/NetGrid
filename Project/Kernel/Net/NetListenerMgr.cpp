/*
 * File:	NetService.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#include "NetListenerMgr.h"
#include "Tools.h"
#include <set>
#include "Tools_time.h"

bool NetListenerMgr::CreateNetListener(IKernel *kernel, const char *ip, s32 port, INetTcpListener *listener)
{
	sockaddr_in stAddr;
	tools::InitSocketAddr(stAddr, ip, port);
	NetSocket netSocket = tools::InitTcpSocket();
	if (netSocket == tools::INVALID_NETSOCKET)
	{
		TRACE_LOG("Init TcpSocket Error, ErrorCode:%d", tools::GetSocketError());
		return false;
	}

	tools::SetSocketNonlock(netSocket);
	tools::SetSocketReuseAddr(netSocket);
	if (!tools::BindSocket(netSocket, stAddr))
	{
		TRACE_LOG("Bind Socket Error, ErrorCode:%d", tools::GetSocketError());
		return false;
	}

	if (!tools::ListenSocket(netSocket))
	{
		TRACE_LOG("Listen Socket Error, ErrorCode:%d", tools::GetSocketError());
		return false;
	}
	TRACE_LOG("Create Listener: netsocket:%d", netSocket);
	OnCreateNetListener(port, netSocket, listener);

	return true;
}

bool NetListenerMgr::CreateNetConnecter(IKernel *kernel, const char *ip, s32 port, ITcpSession *tcpsession)
{
	sockaddr_in stAddr;
	tools::InitSocketAddr(stAddr, ip, port);
	NetSocket netSocket = tools::InitTcpSocket();
	if (netSocket == tools::INVALID_NETSOCKET)
	{
		TRACE_LOG("Init TcpSocket Error, ErrorCode:%d", tools::GetSocketError());
		return false;
	}
	tools::SetSocketNonlock(netSocket);
	NetSocket ret = tools::ConnectSocket(netSocket, stAddr);
	if (tools::INVALID_NETSOCKET == ret)
	{
		TRACE_LOG("Connect Socket Error, ErrorCode:%d", tools::GetSocketError());
		tcpsession->OnError(1, tools::GetSocketError());
		return false;
	}
	if (ret == 0)
		OnCreateNetConnecter(netSocket, stAddr, tcpsession);
	if (ret == netSocket)
		_netService->OnConnectedSocket(_kernel, netSocket, stAddr, tcpsession);
	return true;
}

void NetListenerMgr::Process(IKernel *kernel, s64 tick)
{

	MainEvent evt;
	while (_mainQueue.Pop(evt))
	{
		if (evt.opt == LISTEN_EVT_ADD)
		{
			if (evt.type == LISTENER)
				OnAccept(kernel, evt.listSocket, evt.socket);
			if (evt.type == CONNECTER)
				OnConnect(kernel, evt.socket);
		}
		if (evt.opt == LISTEN_EVT_REMOVE)
		{
			if (evt.type == LISTENER)
			{
				auto iter = _listenSockMap.find(evt.socket);
				ASSERT(iter != _listenSockMap.end(), "error");
				if (iter != _listenSockMap.end())
				{
					iter->second.listener->OnRelease();
					tools::CloseSocket(iter->second.socket);
					_listenSockMap.erase(iter);
				}
			}
			if (evt.type == CONNECTER)
			{
				OnConnectFailed(kernel, evt.socket, evt.listSocket);
			}

		}
	}
}

void NetListenerMgr::OnCreateNetListener(s32 port, NetSocket netSocket, INetTcpListener *listener)
{
	ListenerNode node;
	node.socket = netSocket;
	node.listener = listener;

	_listenSockMap.emplace(netSocket, node);
	StartListen();
	ThreadEvent evt;
	evt.type = LISTENER;
	evt.socket = netSocket;
	evt.opt = LISTEN_EVT_ADD;
	_threadQueue.TryPush(evt);
}

void NetListenerMgr::OnCreateNetConnecter(NetSocket netSocket, sockaddr_in &addr, ITcpSession *tcpsession)
{
	ConnectNode node;
	node.session = tcpsession;
	node.socket = netSocket;
	node.addr = addr;
	_connectSockMap.emplace(netSocket, node);
	StartListen();
	ThreadEvent evt;
	evt.type = CONNECTER;
	evt.socket = netSocket;
	evt.opt = LISTEN_EVT_ADD;
	_threadQueue.TryPush(evt);
}

void NetListenerMgr::StartListen()
{
	if (!_listen)
	{
		_listenThread = std::thread(&NetListenerMgr::Run, this);
		_listen = true;
	}
}

void NetListenerMgr::OnAccept(IKernel *kernel, NetSocket listenSocket, NetSocket clientSocket)
{
	auto iter = _listenSockMap.find(listenSocket);
	ASSERT(iter != _listenSockMap.end(), "error");
	if (iter != _listenSockMap.end())
	{
		ITcpSession *session = iter->second.listener->CreateSession();
		if (nullptr == session)
		{
			tools::CloseSocket(clientSocket);
			return;
		}
		tools::SetSocketNonlock(clientSocket);
		_netService->OnAcceptSocket(kernel, clientSocket, session);
	}
}

void NetListenerMgr::OnConnect(IKernel *kernel, NetSocket connectSocket)
{
	auto iter = _connectSockMap.find(connectSocket);
	ASSERT(iter != _connectSockMap.end(), "error");
	if (iter != _connectSockMap.end())
	{
		_netService->OnConnectedSocket(_kernel, iter->second.socket, iter->second.addr, iter->second.session);
		_connectSockMap.erase(iter);
	}
}

void NetListenerMgr::OnConnectFailed(IKernel *kernel, NetSocket connectSocket, s32 error)
{
	auto iter = _connectSockMap.find(connectSocket);
	ASSERT(iter != _connectSockMap.end(), "error");
	if (iter != _connectSockMap.end())
	{
		iter->second.session->OnError(1, error);
		iter->second.session->OnRelease();
		_connectSockMap.erase(iter);
	}
}

void NetListenerMgr::Run()
{
	IKernel *kernel = _kernel;
	SocketArray netSockets;
	tools::Zero(netSockets);
	fd_set readFd;
	fd_set writFd;
	s32 maxFd = 0;
	s32 nSockets = 0;
	sockaddr_in stClient;
	
	timeval timeOut;
	timeOut.tv_sec = 0;
	timeOut.tv_usec = SELECT_TIME_OUT;

	while (!_terminate)
	{
		//THREAD_LOG("NetListener","NetListenerMgr Running");
		ThreadEvent evt;
		if (_threadQueue.Pop(evt))
		{
			if (evt.opt == LISTEN_EVT_ADD)
			{
				//上层逻辑保护 不会有超过LISTEN_SOCKET_SIZE 数量
				if (netSockets.size < LISTEN_SOCKET_SIZE)
				{
					SocketNode *node = &(netSockets.sockets[netSockets.size++]);
					node->socket = evt.socket;
					node->type = evt.type;
					node->size = 0;
				}
			}
			else if (evt.opt == LISTEN_EVT_REMOVE)
			{
				if (DelSoeckt(netSockets, evt.socket))
				{
					MainEvent mainEvt;
					mainEvt.opt = evt.opt;
					mainEvt.socket = evt.socket;
					mainEvt.type = evt.type;
					_mainQueue.TryPush(mainEvt);
				}
			}
		}

		FD_ZERO(&readFd);
		FD_ZERO(&writFd);
		maxFd = 0;
		if (netSockets.size == 0)
		{
			MSLEEP(SELECT_TIME_OUT);
			continue;
		}
		for (s32 i = 0; i < netSockets.size; i++)
		{
			FD_SET(netSockets.sockets[i].socket, &readFd);
			if (netSockets.sockets[i].type == CONNECTER)
			{
				FD_SET(netSockets.sockets[i].socket, &writFd);
			}
#ifdef LINUX
			maxFd = max(maxFd, netSockets.sockets[i].socket);
#endif
		}
#ifdef LINUX
		maxFd++;
#endif
		s64 tick = tools::GetTimeMillisecond();
		nSockets = select(maxFd, &readFd, &writFd, nullptr, &timeOut);
		if (nSockets < 0)
			THREAD_LOG("Select", "Select exp time:%ld ms, errorcode:%d", tools::GetTimeMillisecond() - tick, tools::GetSocketError());

		std::vector<NetSocket> dels;
		for (s32 i = 0; i < netSockets.size; i++)
		{
				if (netSockets.sockets[i].type == CONNECTER)
				{
					if (FD_ISSET(netSockets.sockets[i].socket, &writFd))
					{
						MainEvent evt;
						evt.socket = netSockets.sockets[i].socket;
						evt.type = CONNECTER;
						evt.opt = LISTEN_EVT_ADD;
						if (FD_ISSET(netSockets.sockets[i].socket, &readFd))
						{
							s32 error = -1;
							tools::QuerySocketError(netSockets.sockets[i].socket, error);
							THREAD_LOG("SocketError", "Query Socket error:%d", error);
							if (error != 0)
							{
								evt.opt = LISTEN_EVT_REMOVE;
								evt.listSocket = error;
								tools::CloseSocket(evt.socket);
							}
						}
						dels.push_back(evt.socket);
						_mainQueue.TryPush(evt);
					}
					else
					{
						if (netSockets.sockets[i].size > CONNECT_TIME_OUT_SIZE)
						{
							MainEvent evt;
							evt.socket = netSockets.sockets[i].socket;
							evt.type = CONNECTER;
							evt.opt = LISTEN_EVT_REMOVE;
#ifdef LINUX
							evt.listSocket = ETIME;
#endif
#ifdef WIN32
							evt.listSocket = WSAETIMEDOUT;
#endif
							dels.push_back(evt.socket);
							_mainQueue.TryPush(evt);
						}
						else
							netSockets.sockets[i].size++;
					}
				}

			if (FD_ISSET(netSockets.sockets[i].socket, &readFd))
			{
				//THREAD_LOG("Listen", "Listen socket:%d, type:%d", netSockets.sockets[i].socket, netSockets.sockets[i].type);
				if (netSockets.sockets[i].type == LISTENER)
				{
					NetSocket clientSocket;
					while (true)
					{
						if (!tools::AcceptSocket(netSockets.sockets[i].socket, stClient, clientSocket))
						{
#ifdef WIN32
							if (tools::GetSocketError() != WSAEWOULDBLOCK)
#endif
#ifdef LINUX
							if (tools::GetSocketError() != EAGAIN)
#endif
								THREAD_LOG("ACCEPT", "%d accept error, code:%d", netSockets.sockets[i].socket, tools::GetSocketError());
							break;
						}
						MainEvent evt;
						evt.socket = clientSocket;
						evt.opt = LISTEN_EVT_ADD;
						evt.type = LISTENER;
						evt.listSocket = netSockets.sockets[i].socket;
						_mainQueue.TryPush(evt);
					}
				}

			}
		}

		for (auto delSoeckt : dels)
			DelSoeckt(netSockets, delSoeckt);

		MSLEEP(SELECT_TIME_OUT);
	}
}

bool NetListenerMgr::DelSoeckt(SocketArray &netSockets, NetSocket delSocket)
{
	s32 i = 0;
	for (; i < netSockets.size; i++)
	{
		if (netSockets.sockets[i].socket == delSocket)
			break;
	}
	if (i != netSockets.size)
	{
		netSockets.sockets[i] = netSockets.sockets[--netSockets.size];
		return true;
	}

	return false;
}