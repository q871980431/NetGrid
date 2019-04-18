/*
 * File:	EchoServer.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/10 15:52:57
 */

#include "EchoSession.h"
void EchoSession::OnEstablish()
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%lld, Enter Establish", _sessionId);
}

void EchoSession::OnTerminate()
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%lld, Enter Terminate", _sessionId);
	DEL this;
}

void EchoSession::OnError(s32 moduleErr, s32 sysErr)
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%lld, Error ModuleError:%d, SysError:%d", _sessionId, moduleErr, sysErr);
}

void EchoSession::OnRecv(s32 messageId, const char *buff, s32 len)
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%lld, Recv MsgId:%d, content:%s len:%d", _sessionId, messageId, buff, len);
	_connection->Send(messageId, buff, len);
}

void EchoClientSession::OnEstablish()
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("EchoClientSession Session:%lld, Enter Establish", _sessionId);
	SendContent();
}

void EchoClientSession::OnRecv(s32 messageId, const char *buff, s32 len)
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("EchoClientSession Session:%lld Recv, Server Rsp: MsgId:%d, messag:%s, len:%d", _sessionId, messageId, buff, len);
	SendContent();
}

void EchoClientSession::SendContent()
{
	if (_sendCount >= 500000)
	{
		_connection->Close();
		return;
	}
	char buff[256] = { 0 };
	SafeSprintf(buff, sizeof(buff), "EchoClientSession:%lld, SendCount:%d", _sessionId, _sendCount++);
	_connection->Send(1101, buff, sizeof(buff));
}