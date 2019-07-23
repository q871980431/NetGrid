#include "HarborSession.h"
#include "Tools_time.h"

s32 HarborSession::OnParsePacket(CircluarBuffer *recvBuff)
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

void HarborSession::OnRecv(const char *buff, s32 len)
{
	MessageHead *head = (MessageHead*)buff;
	ASSERT(head->len == len, "error");
	//TRACE_LOG("Session:%d, Recv MsgId:%d, len:%d", _connection->GetSessionId(), head->messageId, len - (s32)sizeof(MessageHead));
	OnRecv(head->messageId, buff + sizeof(MessageHead), len - sizeof(MessageHead));
}

void HarborClientSession::OnEstablish()
{
	_connecting = false;
	TRACE_LOG("session enter establish, type:%d, nodeId:%d, active close:%d", _nodeType, _nodeId, _activeClose);
	ASSERT(_connection != nullptr, "error");
	if (_activeClose)
		_connection->Close("has active close");
	else
		_harbor->OnOpen(this);
}

void HarborClientSession::OnError(s32 moduleErr, s32 sysErr)
{
	_connecting = false;
}

void HarborClientSession::OnRelease()
{
	if (_activeClose)
	{
		if (_timer)
			_kernel->KillTimer(_timer);

		_harbor->OnHarborClientSessionRelease(this);
		return;
	}
	else
	{
		_connectCount++;
		TRACE_LOG("Try Connect, Now Connect Count:%d", _connectCount);
		ASSERT(_timer == nullptr, "error");
		_timer = NEW ReConnectTimer(this);
		_kernel->StartTimer(_timer, 0, 1, HARBOR_RECONNECT_TIME * tools::MILLISECONDS, "ReConnect Timer");
	}
}

