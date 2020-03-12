#include "ClientMgr.h"
#include "ClientSession.h"

core::ITcpSession * ClientMgr::CreateSession()
{
	Client *client = _clientPool.Create();
	if (client == nullptr)
		return nullptr;

	ClientSession *clientSession = NEW ClientSession(client->GetIndex());
	if (clientSession == nullptr)
	{
		_clientPool.Recover(client);
		return nullptr;
	}
	client->BindSession(clientSession);
	return clientSession;
}
