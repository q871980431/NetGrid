/*
 * File:	NetService.h
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#ifndef __Tools_Net_h__
#define __Tools_Net_h__
#include "MultiSys.h"
#ifdef WIN32
#include <winsock2.h>
#include <WS2tcpip.h>
#endif

#ifdef LINUX
#include<netinet/in.h>
#include <netinet/tcp.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#endif

#ifdef WIN32
typedef SOCKET NetSocket;
#endif

#ifdef LINUX
typedef int NetSocket;
#endif

namespace tools
{
	const static NetSocket INVALID_NETSOCKET = -1;
	const static char *ANY_ADDR_IP = "0";
	inline s32 GetSocketError()
	{
#ifdef LINUX
		return errno;
#endif
#ifdef WIN32
		return WSAGetLastError();
#endif
	}

#ifdef __cplusplus
	extern "C" {
#endif
		//============================START==================================
		void InitSocketAddr(sockaddr_in &stAddr, const char *ip, s32 port);
		//NO Cache
		const char * GetSocketIp(sockaddr_in &st);
		s32			 GetSocketPort(sockaddr_in &st);
		s32			 GetIpV4Addr(const char *ip);

		NetSocket	InitTcpSocket();
		bool BindSocket(NetSocket netSocket, sockaddr_in &stAddr);
		bool ListenSocket(NetSocket netSocket);
		bool AcceptSocket(NetSocket listenSocket, sockaddr_in &sockAddr,NetSocket &clientSocket);
		NetSocket ConnectSocket(NetSocket connectSocket, sockaddr_in &sockAddr);
		bool CloseSocket(NetSocket netSocket);
		void SocketFroceClose(NetSocket netSocket);
		void QuerySocketError(NetSocket netSocket, s32 &error);

		bool SetSocketNonlock(NetSocket netSocket);
		void SetSocketReuseAddr(NetSocket netSocket);
		void GetLocalSocketInfo(NetSocket localSocket, sockaddr_in &sockAddr);
		void GetRomoteSocketInfo(NetSocket romoteSocket, sockaddr_in &sockAddr);
		void SetTcpNodely(NetSocket netSocket);
		//=============================END===================================
#ifdef __cplusplus
	}
#endif
}
#endif