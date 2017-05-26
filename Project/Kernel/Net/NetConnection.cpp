#include "NetConnection.h"
bool NetConnection::IsConnected()
{

	return true;
}

void NetConnection::Send(const char *buff, s32 len)
{

}

void NetConnection::Close()
{

}

NetConnection * NetConnectionMgr::CreateNetConnection()
{
	
	return nullptr;
}