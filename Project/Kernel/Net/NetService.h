/*
 * File:	NetService.h
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#ifndef __NetService_h__
#define __NetService_h__
#include "INetEngine.h"
#include <unordered_map>
#include "NetIOEngine.h"
#include "Tools_Net.h"

using namespace core;

class NetListenerMgr;
class TcpConnection;
typedef std::unordered_map<s32, TcpConnection *> ConnectionMap;
typedef std::unordered_map<NetSocket, s64>	FroceCloseMap;
class NetService : public INetEngine
{
public:
	CREATE_INSTANCE(NetService);
    virtual ~NetService(){};

	virtual bool Ready();
    virtual bool Initialize();
    virtual bool Destroy();

	virtual void Process(core::IKernel *kernel, s32 tick);
	virtual void CreateNetListener(const char *ip, s32 port, INetTcpListener *listener);
	virtual void CreateNetSession(const char *ip, s32 port, ITcpSession *session);

public:
	void OnAcceptSocket(IKernel *kernel, NetSocket clientSocket, ITcpSession *session);
	void OnConnectedSocket(IKernel *kernel, NetSocket connectSocket, sockaddr_in &addr, ITcpSession *session);
public:
	TcpConnection * CreateConnection(s32 sessionId, bool passive, NetSocket netSocket, ITcpSession *session);
	void RemoveConnection(s32 sessionId, bool recvFin);
	IKernel * GetKernel() { return s_kernel; };
protected:
private:
	s32	GetSessionId() { return ++_sessionId; };
private:
    static NetService     * s_self;
    static IKernel  * s_kernel;
	NetListenerMgr	* _listenerMgr;
	s32				  _sessionId;
	ConnectionMap	  _connectionMap;
	NetIOEngine		  *_netIoEngine;
};
#endif