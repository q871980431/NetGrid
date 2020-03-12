#ifndef __CLIENT_MGR_H__
#define __CLIENT_MGR_H__
#include "MultiSys.h"
#include "Tools.h"
#include "TIndexPool.h"
#include "Client.h"

typedef tlib::IndexPool<Client> ClientPool;
class ClientMgr : public core::INetTcpListener
{
public:
	ClientMgr() {};

	virtual core::ITcpSession * CreateSession();
	virtual void OnRelease() {};

	void RecoverClient(Client *client) { _clientPool.Recover(client); };
protected:
private:
	ClientPool _clientPool;
};

#define CLIENTMGR tools::Singleton<ClientMgr>::Instance()

#endif
