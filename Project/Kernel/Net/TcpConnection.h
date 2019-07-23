/*
 * File:	NetService.h
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#ifndef __TcpConnection_h__
#define __TcpConnection_h__
#include "INetEngine.h"
#include "Tools_Net.h"
using namespace core;
class NetListenerMgr;
class IIODriver;
class IIOEngine;
class NetService;
class TcpConnection : public core::ITcpConnection
{
public:
	TcpConnection(s32 sessionId, bool passive, NetSocket netSocket, ITcpSession *session, NetService *service);
    virtual ~TcpConnection(){};

	virtual bool  IsPassiveConnection(void) { return _passive; };
	virtual void  Send(const char *buff, s32 len);      
	virtual void  Close(const char *reason);                              //πÿ±’¡¨Ω”
	virtual const char * GetRemoteIP();
	virtual s32   GetRemoteIpAddr();
	virtual bool  SettingBuffSize(s32 recvSize, s32 sendSize);
	virtual s32   GetSessionId() { return _sessionId; };
	virtual const char * GetCloseReason();

public:
	void Init();
	inline NetSocket GetNetSocket() { return _netSocket; }
	void OnEstablish();
	void OnTerminate(bool recvFin, s32 errorCode);
	void OnRecv(IKernel *kernel);

public:

	inline void SetIOEngine(IIOEngine *ioEngine) { _ioEngine = ioEngine; };
	inline IIOEngine * GetIOEngine() { return _ioEngine; };
	inline void SetIODriver(IIODriver *ioDriver) { _ioDriver = ioDriver; };
	inline IIODriver * GetIODriver() { return _ioDriver; };
	inline void SetSendBuffSize(s32 sendBuffSize) { _sendBuffSize = sendBuffSize; };
	inline s32	GetSendBuffSize() { return _sendBuffSize; };
	inline void SetRecvBuffSize(s32 recvBuffSize) { _recvBuffSize = recvBuffSize; };
	inline s32	GetRecvBuffSize() { return _recvBuffSize; };

private:
	s32	 _sessionId;
	bool _passive;
	NetSocket _netSocket;
	IIODriver *_ioDriver;
	IIOEngine *_ioEngine;
	s32	 _sendBuffSize;
	s32	 _recvBuffSize;
	bool _establish;

	ITcpSession *_tcpSession;
	NetService	*_service;
	sockaddr_in	 _romoteAddr;
	std::string	 _closeReason;
};
#endif