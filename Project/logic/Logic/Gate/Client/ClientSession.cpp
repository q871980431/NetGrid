#include "ClientSession.h"
#include "../Gate.h"
#include "ClientMgr.h"

s32 ClientSession::s_sendBuffSize = 64 * 1024;
s32 ClientSession::s_recvBuffSIze = 64 * 1024;

void ClientSession::OnEstablish()
{
	ASSERT(_connection != nullptr, "error");
	TRACE_LOG("GateSession:%d, Establish, connection:%d", _indexId, _connection != nullptr);
	if (_connection)
	{
		_connection->SettingBuffSize(s_recvBuffSIze, s_sendBuffSize);
		Gate::OnClientEnter(_client);
	}
}

void ClientSession::OnRecv(const char *buff, s32 len)
{
	Gate::OnClientMsg(_client, buff, len);
}

void ClientSession::OnError(s32 moduleErr, s32 sysErr)
{
	TRACE_LOG("client session error, module error:%d, sys error:%d", moduleErr, sysErr);
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
	Gate::OnClientLeave(_client);
	CLIENTMGR.RecoverClient(_client);
}

void ClientSession::Close(const char *reason)
{
	if (_connection)
		_connection->Close(reason);
}