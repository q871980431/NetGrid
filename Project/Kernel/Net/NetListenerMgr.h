/*
 * File:	NetService.h
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#ifndef __NetListenerMgr_h__
#define __NetListenerMgr_h__
#include "NetService.h"
#include "Tools_Net.h"
#include "CircularQueue.h"
#include <map>
#include <thread>

class NetListenerMgr
{
	const static s32 LISTEN_SOCKET_SIZE = 64;
	const static s32 SELECT_TIME_OUT = 10;
	const static s32 CONNECT_TIME_OUT = 10;
	const static s32 CONNECT_TIME_OUT_SIZE = CONNECT_TIME_OUT * (1000 / (2*10));
	struct ListenerNode 
	{
		NetSocket socket;
		INetTcpListener * listener;
	};

	struct ConnectNode 
	{
		NetSocket socket;
		ITcpSession *session;
		sockaddr_in addr;
	};

	enum SOCKET_TYPE{
		LISTENER = 1,
		CONNECTER = 2,
	};
	enum LISTEN_OPT{
		LISTEN_EVT_ADD = 1,
		LISTEN_EVT_REMOVE = 2,
	};

	struct ThreadEvent 
	{
		SOCKET_TYPE type;
		NetSocket socket;
		LISTEN_OPT opt;
	};

	struct MainEvent 
	{
		SOCKET_TYPE type;
		NetSocket socket;
		LISTEN_OPT opt;
		NetSocket listSocket;
	};

	struct AcceptEvent 
	{
		NetSocket listenSocket;
		NetSocket acceptSocket;
	};

	struct SocketNode 
	{
		SOCKET_TYPE type;
		NetSocket socket;
		s32	size;
	};

	struct SocketArray 
	{
		SocketNode sockets[LISTEN_SOCKET_SIZE];
		s32 size;
	};

	//Key = netsocket
	typedef std::map<s32, ListenerNode> ListenSockMap;
	typedef std::map<s32, ConnectNode> ConnectSockMap;
	typedef CircluarQueue<ThreadEvent> EventQueue;
	typedef CircluarQueue<MainEvent>   MainQueue;

public:
	NetListenerMgr(NetService *netService, IKernel *kernel) :_netService(netService), _kernel(kernel), _listen(false), _terminate(false), _threadQueue(128),_mainQueue(64) {};
	~NetListenerMgr() {};

	bool CreateNetListener(IKernel *kernel, const char *ip, s32 port, INetTcpListener *listener);
	bool CreateNetConnecter(IKernel *kernel, const char *ip, s32 port, ITcpSession *tcpsession);

	void Process(IKernel *kernel, s64 tick);
private:
	void OnCreateNetListener(s32 port, NetSocket netSocket, INetTcpListener *listener);
	void OnCreateNetConnecter(NetSocket netSocket, sockaddr_in &addr, ITcpSession *tcpsession);
	void StartListen();
	void OnAccept(IKernel *kernel, NetSocket listenSocket, NetSocket clientSocket);
	void OnConnect(IKernel *kernel, NetSocket connectSocket);
	void OnConnectFailed(IKernel *kernel, NetSocket connectSocket, s32 error);

private:
	//Running in thread functions
	void Run();
	bool DelSoeckt(SocketArray &socketArray, NetSocket delSocket);
protected:
private:
	NetService	*_netService;
	IKernel *_kernel;
	ListenSockMap _listenSockMap;
	ConnectSockMap _connectSockMap;

	bool	_listen;
	std::thread _listenThread;
	bool	_terminate;
	EventQueue	_threadQueue;
	MainQueue	_mainQueue;
};
#endif