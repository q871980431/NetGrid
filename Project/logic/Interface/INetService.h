/*
 * File:	INetService.h
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#ifndef  __INetService_h__
#define __INetService_h__
#include "IModule.h"
#include "CircularBuffer.h"
#ifdef WIN32
typedef SOCKET NetSocket;
#endif

#ifdef LINUX
typedef int NetSocket;
#endif

class ITcpConnection
{
public:
	const static s32 INIT_SEND_BUFF_SIZE = 1 * 1024;
	const static s32 INIT_RECV_BUFF_SIZE = 1 * 1024;
	const static s32 CLOSE_DELAY_TIME = 5;			//延迟关闭5S

	virtual bool  IsPassiveConnection(void) = 0;			//是否是被动链接
	virtual void  Send(const char *buff, s32 len) = 0;      //
	virtual void  Close() = 0;                              //关闭连接
	virtual const char * GetRemoteIP() = 0;
	virtual s32   GetRemoteIpAddr() = 0;
	virtual void  SettingBuffSize(s32 recvSize, s32 sendSize) = 0;	//设置时 需保证无数据发送和接收, 可以在SetConnection时进行调用
	virtual s32   GetSessionId() = 0;
};

class ITcpSession
{
public:
	const static s32 INVALID_PACKET_LEN = -1;
	const static s32 PACKET_UNFULL = 0;

	virtual void  SetConnection(ITcpConnection *connection) = 0;
	virtual void  OnEstablish() = 0;
	virtual void  OnTerminate() = 0;
	virtual void  OnError(s32 moduleErr, s32 sysErr) = 0;
	virtual void  OnRecv(const char *buff, s32 len) = 0;
	virtual s32	  OnParsePacket(CircluarBuffer *recvBuff) = 0;
};

class INetTcpListener
{
public:
	virtual ITcpSession * CreateSession() = 0;
	virtual void OnRelease() = 0;
};

class INetService : public IModule
{
public:
    virtual ~INetService(){};
    
	virtual void CreateNetListener(const char *ip, s32 port, INetTcpListener *listener) = 0;
	virtual void CreateNetConnecter(const char *ip, s32 port, ITcpSession *session) = 0;
};

#endif