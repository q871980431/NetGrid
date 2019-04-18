/*
 * File:	EchoServer.h
 * Author:	xuping
 * 
 * Created On 2019/4/10 15:52:57
 */

#ifndef __EchoSession_h__
#define __EchoSession_h__
#include "IKernel.h"

class EchoSession : public core::IMsgSession
{
public:
	EchoSession(core::IKernel *kernel, s64 sessionId):_sessionId(sessionId),_kernel(kernel) {};
    virtual ~EchoSession(){};

	virtual void  SetConnection(core::IMsgConnection *connection) { _connection = connection; };
	virtual void  OnEstablish();
	virtual void  OnTerminate();
	virtual void  OnError(s32 moduleErr, s32 sysErr);
	virtual void  OnRecv(s32 messageId, const char *buff, s32 len);
protected:
	core::IMsgConnection *_connection;
	s64 _sessionId;
	core::IKernel *_kernel;
};

class EchoClientSession : public EchoSession
{
public:
	EchoClientSession(core::IKernel *kernel, s64 sessionId) :EchoSession(kernel, sessionId), _sendCount(0) {};

	virtual void  OnEstablish();
	virtual void  OnRecv(s32 messageId, const char *buff, s32 len);
protected:
	void SendContent();
private:
	s32 _sendCount;
};
#endif