/*
 * File:	NetService.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#include "Tools_Net.h"
#ifdef __cplusplus
extern "C" {
#endif
	//===================================START===============================
	void InitSocketAddr(sockaddr_in &stAddr, const char *ip, s32 port)
	{
		stAddr.sin_port = htons(port);
		stAddr.sin_family = AF_INET;
		if (0 == strcmp(ip, tools::ANY_ADDR_IP))
			stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		else {
			inet_pton(AF_INET, ip, &stAddr.sin_addr);
		}
	}

	const char * GetSocketIp(sockaddr_in &st)
	{
		thread_local static char ip[32];
		if (inet_ntop(AF_INET, &st.sin_addr, ip, sizeof(ip)) != nullptr)
			return ip;
		return "";
	}

	s32 GetSocketPort(sockaddr_in &st)
	{
		return ntohs(st.sin_port);
	}

	s32 GetIpV4Addr(const char *ip)
	{
		s32 ret = 0;
		inet_pton(AF_INET, ip, &ret);
		return ret;
	}
	
	NetSocket InitTcpSocket()
	{
		NetSocket ret = socket(AF_INET, SOCK_STREAM, 0);
#ifdef WIN32
		if (ret == INVALID_SOCKET)
#endif
#ifdef LINUX
		if (ret == -1)
#endif
			return tools::INVALID_NETSOCKET;
		return ret;
	}
	
	bool BindSocket(NetSocket netSocket, sockaddr_in &stAddr)
	{
#ifdef WIN32
		if (bind(netSocket, (sockaddr*)&stAddr, sizeof(stAddr)) == SOCKET_ERROR)
#endif
#ifdef LINUX
		if (bind(netSocket, (sockaddr*)&stAddr, sizeof(stAddr)) != 0)
#endif
			return false;
		return true;
	}

	bool ListenSocket(NetSocket netSocket)
	{
#ifdef WIN32
		if (listen(netSocket, SOMAXCONN) == SOCKET_ERROR)
#endif
#ifdef LINUX
		if (listen(netSocket, 1) != 0)
#endif
				return false;
		return true;
	}

	bool AcceptSocket(NetSocket listenSocket, sockaddr_in &sockAddr, NetSocket &clientSocket)
	{
		NetSocket temp;
		socklen_t len = sizeof(sockAddr);
		temp = accept(listenSocket, (sockaddr *)&sockAddr, &len);

#ifdef WIN32
		if (temp == INVALID_SOCKET)		
#endif
#ifdef LINUX
		if (temp == - 1)
#endif
			return false;
		clientSocket = temp;
		return true;
	}

	NetSocket ConnectSocket(NetSocket connectSocket, sockaddr_in &sockAddr)
	{
#ifdef WIN32
		if (connect(connectSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
		{
			if (tools::GetSocketError() == WSAEWOULDBLOCK)
				return 0;
			return tools::INVALID_NETSOCKET;
		}
#endif
#ifdef LINUX
		if (connect(connectSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == -1)
		{
			if (tools::GetSocketError() == EINPROGRESS)
				return 0;
			return tools::INVALID_NETSOCKET;
		}
#endif
		return connectSocket;
	}

	bool CloseSocket(NetSocket netSocket)
	{
#ifdef WIN32
		if (closesocket(netSocket) == SOCKET_ERROR)
#endif
#ifdef LINUX
			if (close(netSocket) == -1)
#endif
				return false;
		return true;
	}

	void SocketFroceClose(NetSocket netSocket)
	{
		linger stLinger;
		stLinger.l_onoff = 1;
		stLinger.l_linger = 0;
		setsockopt(netSocket, SOL_SOCKET, SO_LINGER, (const char *)&stLinger, sizeof(stLinger));
		CloseSocket(netSocket);
	}

	void QuerySocketError(NetSocket netSocket, s32 &error)
	{
		socklen_t len = sizeof(error);
#ifdef WIN32
		getsockopt(netSocket, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
#endif
#ifdef LINUX
		getsockopt(netSocket, SOL_SOCKET, SO_ERROR, &error, &len);
#endif
	}


	bool SetSocketNonlock(NetSocket netSocket)
	{
#ifdef WIN32
		s32 imode = 1;
		if (ioctlsocket(netSocket, FIONBIO, (u_long *)&imode) == SOCKET_ERROR)
#endif
#ifdef LINUX
		if (fcntl(netSocket, F_SETFL, fcntl(netSocket, F_GETFL)| O_NONBLOCK) == -1)
#endif
			return false;
		return true;
	}

	void SetSocketReuseAddr(NetSocket netSocket)
	{
#ifdef WIN32
		BOOL bReuseaddr = TRUE;
		setsockopt(netSocket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, (const char *)&bReuseaddr, sizeof(bReuseaddr));
		setsockopt(netSocket, SOL_SOCKET, SO_REUSEADDR, (const char *)&bReuseaddr, sizeof(bReuseaddr));
#endif
#ifdef LINUX
		int on = 1;
		setsockopt(netSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#endif
	}


	void GetLocalSocketInfo(NetSocket localSocket, sockaddr_in &sockAddr)
	{
		socklen_t len = sizeof(sockAddr);
		getsockname(localSocket, (sockaddr*)&sockAddr, &len);
	}

	void GetRomoteSocketInfo(NetSocket romoteSocket, sockaddr_in &sockAddr)
	{
		socklen_t len = sizeof(sockAddr);
		getpeername(romoteSocket, (sockaddr*)&sockAddr, &len);
	}

	void SetTcpNodely(NetSocket netSocket)
	{
#ifdef WIN32
		BOOL on = TRUE;
		setsockopt(netSocket, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(on));
#endif

#ifdef LINUX
		int on = 1;
		setsockopt(netSocket, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
#endif
	}

	//===================================END======================================
#ifdef __cplusplus
}
#endif

