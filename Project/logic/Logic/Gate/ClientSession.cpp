#include "ClientSession.h"

s32 ClientSession::s_sendBuffSize = 0;
s32 ClientSession::s_recvBuffSIze = 0;

void ClientSession::OnEstablish()
{
	ASSERT(_connection != nullptr, "error");
	TRACE_LOG("GateSession:%d, Establish, connection:%d", _indexId, _connection != nullptr);
	if (_connection)
	{
		_connection->SettingBuffSize(s_recvBuffSIze, s_sendBuffSize);
	}
}

void ClientSession::OnRecv(const char *buff, s32 len)
{

}

void ClientSession::OnError(s32 moduleErr, s32 sysErr)
{

}

s32 ClientSession::OnParsePacket(CircluarBuffer *recvBuff)
{
	ClientMsgHead head;
	ClientMsgHead *temp = (ClientMsgHead*)recvBuff->TryReadBuff(&head, sizeof(head));
	if (!temp)
		return ITcpSession::PACKET_UNFULL;
	if (temp->len <= 0 || temp->len > 64 * 1024)
		return ITcpSession::INVALID_PACKET_LEN;
	s32 len = recvBuff->DataSize();
	if (len >= temp->len)
		return temp->len;

	return ITcpSession::PACKET_UNFULL;
}

void ClientSession::OnRelease()
{

}

void ClientSession::Close(const char *reason)
{
	if (_client)
	{

		_client = nullptr;
	}
	if (_connection)
		_connection->Close(reason);
}