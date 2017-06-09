#include "NetConnection.h"
NetConnection::NetConnection(s32 connectionId) :_connectionId(connectionId)
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

void NetConnection::Send(const char *buff, s32 len)
{
    bufferevent_write(_buffEvent, buff, len);
}

void NetConnection::OnSend()
{
    struct evbuffer *eb = bufferevent_get_output(_buffEvent);
    size_t num = evbuffer_get_length(eb);
    if ( num == 0 && _doClose)
    {
        bufferevent_free(_buffEvent);
        _buffEvent = nullptr;
    }
}
void NetConnection::Close()
{
    _doClose = true;
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
    NetConnection *connetion = NEW NetConnection(++_connectionId);
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
