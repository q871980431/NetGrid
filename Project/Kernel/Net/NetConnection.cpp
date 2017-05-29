#include "NetConnection.h"
bool NetConnection::IsConnected()
{

	return true;
}

void NetConnection::Send(const char *buff, s32 len)
{
    bufferevent_write(_buffEvent, buff, len);
}

void NetConnection::Close()
{
    _doClose = true;
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