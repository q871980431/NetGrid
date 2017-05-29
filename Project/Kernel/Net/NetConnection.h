#ifndef __NetConnection_h__
#define __NetConnection_h__
#include "IKernel.h"
#include <unordered_map>

#include <event2/event.h>
#include <event2/buffer.h>  
#include <event2/bufferevent.h>  
#include <event2/event_struct.h>

class NetConnection : public core::ITcpConnection
{
public:
	NetConnection(s32 connectionId) :_connectionId(connectionId) {};
	virtual ~NetConnection(){};

	virtual bool  IsConnected(void);
	virtual void  Send(const char *buff, s32 len);      
    virtual void  Close();

public:
    inline void SetBuffEvent(struct bufferevent *buffEvent){ _buffEvent = buffEvent; };
    inline void SetSession(core::ITcpSession *session){ _session = session; };
    inline core::ITcpSession * GetSession(){ return _session; };
    inline s32  GetConnetionID(){ return _connectionId; };
protected:
private:
	s32	                    _connectionId;
    struct bufferevent    * _buffEvent;
    core::ITcpSession     * _session;
    bool                    _doClose;
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