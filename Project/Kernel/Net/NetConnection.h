#ifndef __NetConnection_h__
#define __NetConnection_h__
#include "IKernel.h"
#include <unordered_map>

class NetConnection : public core::ITcpConnection
{
public:
	NetConnection(s32 connectionId) :_connectionId(connectionId) {};

	virtual ~NetConnection(){};

	virtual bool  IsConnected(void);
	virtual void  Send(const char *buff, s32 len);      
	virtual void  Close();                              

protected:
private:
	s32	_connectionId;
};

typedef std::unordered_map<s32, NetConnection *>	ConnectionMap;
class NetConnectionMgr
{
public:
	NetConnectionMgr() {};
	~NetConnectionMgr() {};

	NetConnection * CreateNetConnection();
	void RealseConnection(NetConnection *connection);

protected:
private:
	ConnectionMap	_indexMap;
	s32				_connectionId;
};
#endif