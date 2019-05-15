/*
 * File:	EchoServer.h
 * Author:	xuping
 * 
 * Created On 2019/4/10 15:52:57
 */

#ifndef __ServiceSession_h__
#define __ServiceSession_h__
#include "IKernel.h"
using namespace core;
class EchoSession : public ITcpSession
{
public:
	EchoSession(core::IKernel *kernel):_kernel(kernel) {};
	virtual ~EchoSession() {};

	virtual void  SetConnection(ITcpConnection *connection) { _connection = connection; 
	if (_connection)
		_sessionId = _connection->GetSessionId();
	};
	virtual void  OnEstablish();
	virtual void  OnTerminate();
	virtual void  OnError(s32 moduleErr, s32 sysErr);
	virtual void  OnRecv(const char *buff, s32 len);
	virtual s32	  OnParsePacket(CircluarBuffer *recvBuff);

protected:
	void SendMsg(s32 msgId, const char *buff, s32 len);

protected:
	ITcpConnection *_connection;
	s64 _sessionId;
	core::IKernel *_kernel;
};

class EchoClientSession : public EchoSession
{
public:
	EchoClientSession(core::IKernel *kernel, bool clientClose) :EchoSession(kernel), _sendCount(0), _clientClose(clientClose) {};

	virtual void  OnEstablish();
	virtual void  OnRecv(const char *buff, s32 len);
	virtual void  OnError(s32 moduleErr, s32 sysErr);
protected:
	void SendContent();
private:
	s32		_sendCount;
	bool	_clientClose;
};
#endif