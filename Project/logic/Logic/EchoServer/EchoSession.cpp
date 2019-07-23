/*
 * File:	EchoServer.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/10 15:52:57
 */

#include "EchoSession.h"
using namespace core;

void EchoSession::OnEstablish()
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%ld, Enter Establish, RemoteIp:%s", _sessionId, _connection->GetRemoteIP());
}

void EchoSession::OnTerminate()
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%ld, Enter Terminate", _sessionId);
}

void EchoSession::OnError(s32 moduleErr, s32 sysErr)
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%ld, Error ModuleError:%d, SysError:%d", _sessionId, moduleErr, sysErr);
}

s32 EchoSession::OnParsePacket(CircluarBuffer *recvBuff)
{
	MessageHead head;
	MessageHead *temp = (MessageHead*)recvBuff->TryReadBuff(&head, sizeof(head));
	if (!temp)
		return ITcpSession::PACKET_UNFULL;
	if (temp->len <= 0 || temp->len > 64 * 1024)
		return ITcpSession::INVALID_PACKET_LEN;
	s32 len = recvBuff->DataSize();
	if (len >= temp->len)
		return temp->len;

	return ITcpSession::PACKET_UNFULL;
}

void EchoSession::OnRecv(const char *buff, s32 len)
{
	MessageHead *head = (MessageHead*)buff;
	ASSERT(head->len == len, "error");
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%d, Recv MsgId:%d, content:%s len:%d", _connection->GetSessionId(), head->messageId, buff + sizeof(MessageHead), len - (s32)sizeof(MessageHead));
	OnRecv(head->messageId, buff + sizeof(MessageHead), len - sizeof(MessageHead));
}

void EchoSession::OnRecv(s32 messageId, const char *buff, s32 len)
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%ld, Recv MsgId:%d, content:%s len:%d", _sessionId, messageId, buff, len);
	SendMsg(messageId, buff, len);
}

void EchoSession::SendMsg(s32 msgId, const char *buff, s32 len)
{
	MessageHead head;
	head.messageId = msgId;
	head.len = len + sizeof(MessageHead);
	_connection->Send((const char *)&head, sizeof(MessageHead));
	_connection->Send(buff, len);
}

void EchoClientSession::OnEstablish()
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("EchoClientSession Session:%ld, Enter Establish", _sessionId);
	SendContent();
}

void EchoClientSession::OnRecv(s32 messageId, const char *buff, s32 len)
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("EchoClientSession Session:%ld Recv, Server Rsp: MsgId:%d, messag:%s, len:%d", _sessionId, messageId, buff, len);
	SendContent();
}

void EchoClientSession::SendContent()
{
	if (_sendCount >= 500000)
	{
		_connection->Close("sendcount > 500000");
		return;
	}
	char buff[256] = { 0 };
	SafeSprintf(buff, sizeof(buff), "EchoClientSession:%ld, SendCount:%d", _sessionId, _sendCount++);
	SendMsg(1101, buff, sizeof(buff));
}