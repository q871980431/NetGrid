/*
 * File:	NetServer.cpp
 * Author:	xuping
 * 
 * Created On 2017/5/29 21:06:25
 */

#include "NetServer.h"
#include "Tools_time.h"
NetServer * NetServer::s_self = nullptr;
IKernel * NetServer::s_kernel = nullptr;
NetListener NetServer::s_netListener;
GameSession NetServer::s_session;


s32 GameSession::OnParsePacket(CircluarBuffer *recvBuff)
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

void GameSession::OnRecv(const char *buff, s32 len)
{
	MessageHead *head = (MessageHead*)buff;
	ASSERT(head->len == len, "error");
	//core::IKernel *kernel = _kernel;
	//TRACE_LOG("Session:%lld, Recv MsgId:%d, content:%s len:%d", _sessionId, head->messageId, buff + sizeof(MessageHead), len - sizeof(MessageHead));
	OnRecv(head->messageId, buff + sizeof(MessageHead), len - sizeof(MessageHead));
}

void GameSession::OnRecv(s32 messageId, const char *buff, s32 len)
{
	s64 now = tools::GetTimeMillisecond();
    SendMsg(messageId, buff, len);
	if (_count == 0)
		_first = now;
	++_count;
	if (_count % 100000 == 0)
	{
		ECHO("%ld ms", now - _first);
		_first = now;
	}
}

void GameSession::SendMsg(s32 msgId, const char *buff, s32 len)
{
	MessageHead head;
	head.messageId = msgId;
	head.len = len + sizeof(MessageHead);
	_connection->Send((const char *)&head, sizeof(MessageHead));
	_connection->Send(buff, len);
}


bool NetServer::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool NetServer::Launched(IKernel *kernel)
{
    //s_kernel->CreateNetListener("0", 11401, &s_netListener);
    //s_kernel->CreateNetSession("192.168.124.1", 11401, &s_session);

    return true;
}

bool NetServer::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


