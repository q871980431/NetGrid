/*
 * File:	EchoServer.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/10 15:52:57
 */
#include "ServiceSession.h"

void EchoSession::OnEstablish()
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%ld, Enter Establish, RemoteIp:%s", _sessionId, _connection->GetRemoteIP());

	//char buff[256] = { 0 };
	//SafeSprintf(buff, sizeof(buff), "Hello My is froce close server");
	//SendMsg(1101, buff, sizeof(buff));
	//_connection->Close();
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

void EchoSession::OnRecv(const char *buff, s32 len)
{
	MessageHead *head = (MessageHead*)buff;
	ASSERT(head->len == len, "error");
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%ld, Recv MsgId:%d, content:%s len:%d", _sessionId, head->messageId, buff+(s32)sizeof(MessageHead), len-(s32)sizeof(MessageHead));
	if (head->messageId == 1102)	//服务器主动断开
	{
		TRACE_LOG("Recv Server close msg");
		_connection->Close("recv close msg");
	}
	else
	{
		_connection->Send(buff, len);
	}
}

s32 EchoSession::OnParsePacket(CircluarBuffer *recvBuff)
{
	MessageHead head;
	MessageHead *temp = (MessageHead*)recvBuff->TryReadBuff(&head, sizeof(head));
	if (!temp)
		return ITcpSession::PACKET_UNFULL;
	if (temp->len <=0 || temp->len > 64 * 1024)
		return ITcpSession::INVALID_PACKET_LEN;
	s32 len = recvBuff->DataSize();
	if (len >= temp->len)
		return temp->len;

	return ITcpSession::PACKET_UNFULL;
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

void EchoClientSession::OnRecv(const char *buff, s32 len)
{
	core::IKernel *kernel = _kernel;
	MessageHead *head = (MessageHead*)buff;
	ASSERT(head->len == len, "error");
	TRACE_LOG("EchoClientSession Session:%ld, Recv  Server Rsp MsgId:%d, content:%s len:%d", _sessionId, head->messageId, buff + (s32)sizeof(MessageHead), len - (s32)sizeof(MessageHead));
	SendContent();
}

void EchoClientSession::SendContent()
{
	if (_sendCount >= 5)
	{
		if (_clientClose)
			_connection->Close("client close");
		else
		{
			SendMsg(1102, nullptr, 0);
		}
		return;
	}
	char buff[256] = { 0 };
	SafeSprintf(buff, sizeof(buff), "EchoClientSession:%ld, SendCount:%d", _sessionId, _sendCount++);
	SendMsg(1101, buff, sizeof(buff));
}

void EchoClientSession::OnError(s32 moduleErr, s32 sysErr)
{
	core::IKernel *kernel = _kernel;
	TRACE_LOG("EchoClientSession Enter Error:code:%d", sysErr);
}