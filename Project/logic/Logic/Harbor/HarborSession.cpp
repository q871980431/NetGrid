#include "HarborSession.h"
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
	core::IKernel *kernel = _kernel;
	TRACE_LOG("Session:%d, Recv MsgId:%d, content:%s len:%d", _connection->GetSessionId(), head->messageId, buff + sizeof(MessageHead), len - sizeof(MessageHead));
	OnRecv(head->messageId, buff + sizeof(MessageHead), len - sizeof(MessageHead));
}