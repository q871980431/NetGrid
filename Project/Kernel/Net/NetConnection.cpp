#include "NetConnection.h"
#include "NetEngine.h"
#include "../Kernel.h"
#ifdef LINUX
#include <arpa/inet.h>
#endif

NetConnection::NetConnection(NetConnectionMgr *connectionMgr, s32 connectionId) :_connectionId(connectionId),
_connetionMgr(connectionMgr)
{
    _buffEvent = nullptr;
    _session = nullptr;
    _doClose = false;
    _reciveSize = 64 * 1024;
    _sendSize = 64 * 1024;
}

bool NetConnection::IsConnected()
{

	return true;
}

void NetConnection::Send(s32 messageId, const char *buff, s32 len)
{
    core::MessageHead head;
    head.messageId = messageId;
    head.len = len + sizeof(head);

    struct evbuffer *eb = bufferevent_get_output(_buffEvent);
    size_t num = evbuffer_get_length(eb);
    if (num + head.len > _sendSize)
    {
        ForceClose();
        return;
    }
    evbuffer_add(eb, &head, sizeof(head));
    evbuffer_add(eb, buff, len);
}

void NetConnection::SendBuff(const char *buff, s32 len)
{
    struct evbuffer *eb = bufferevent_get_output(_buffEvent);
    size_t num = evbuffer_get_length(eb);
    if (num + len > _sendSize)
    {
        ForceClose();
        return;
    }

    evbuffer_add(eb, buff, len);
}

void NetConnection::OnSend()
{
    struct evbuffer *eb = bufferevent_get_output(_buffEvent);
    size_t num = evbuffer_get_length(eb);
    if (num == 0 && _doClose)
        ForceClose();
}

void NetConnection::OnReceive(s32 messageId, void *content, s32 size)
{
    if (_session != nullptr)
        _session->OnRecv(messageId, (const char *)content, size);
}


void NetConnection::ForceClose()
{
    bufferevent_free(_buffEvent);
    if (nullptr != _session)
    {
        _session->SetConnection(nullptr);
        _session->OnTerminate();
    }
    _connetionMgr->RealseConnection(this);
}
void NetConnection::Close()
{
    _doClose = true;
	OnSend();
}

const char * NetConnection::GetRemoteIP()
{
    return inet_ntoa(_romoteAddr.sin_addr);
}

s32 NetConnection::GetRemoteIpAddr()
{
	return _romoteAddr.sin_addr.s_addr;
}

void NetConnection::SettingBuffSize(s32 recvSize, s32 sendSize)
{
    _reciveSize = recvSize;
    _sendSize = sendSize;
    if (_buffEvent != nullptr)
    {
        bufferevent_setwatermark(_buffEvent, EV_READ, 0, _reciveSize);
        bufferevent_setwatermark(_buffEvent, EV_WRITE, 0, _sendSize);
    }
}

void NetConnection::SetBuffEvent(struct bufferevent *buffEvent)
{
    _buffEvent = buffEvent;
    if (buffEvent != nullptr)
    {
        bufferevent_setwatermark(buffEvent, EV_READ, 0, _reciveSize);
        bufferevent_setwatermark(buffEvent, EV_WRITE, 0, _sendSize);
    }
}


NetConnection * NetConnectionMgr::CreateNetConnection()
{
    NetConnection *connetion = NEW NetConnection(this, ++_connectionId);
    _indexMap.insert(std::make_pair(_connectionId, connetion));

	return connetion;
}

void NetConnectionMgr::RealseConnection(NetConnection *connection)
{
    auto iter = _indexMap.find(connection->GetConnetionID());
    if (iter != _indexMap.end())
    {
        _indexMap.erase(iter);
        DEL connection;
    }
}
