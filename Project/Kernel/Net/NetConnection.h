#ifndef __NetConnection_h__
#define __NetConnection_h__
#include "IKernel.h"
#include <unordered_map>

#include <event2/event.h>
#include <event2/buffer.h>  
#include <event2/bufferevent.h>  
#include <event2/event_struct.h>

struct MessageHead
{
    s32 messageId;
    s32 len;
};
class NetConnectionMgr;
class NetConnection : public core::ITcpConnection
{
public:
    NetConnection(NetConnectionMgr *connectionMgr, s32 connectionId);
	virtual ~NetConnection(){};

	virtual bool  IsConnected(void);
    virtual void  Send(s32 messageId, const char *buff, s32 len);
    virtual void  Close();

    void OnSend();
    void OnReceive(s32 messageId, void *content, s32 size);
public:
    void ForceClose();

    void SetBuffEvent(struct bufferevent *buffEvent);
    inline void SetSession(core::ITcpSession *session){ _session = session; };
    inline core::ITcpSession * GetSession(){ return _session; };
    inline s32  GetConnetionID(){ return _connectionId; };
protected:
private:
    NetConnectionMgr        *_connetionMgr;
	s32	                    _connectionId;
    struct bufferevent    * _buffEvent;
    core::ITcpSession     * _session;
    bool                    _doClose;
    s32                     _reciveSize;
    s32                     _sendSize;
};

typedef std::unordered_map<s32, NetConnection *>	ConnectionMap;
class NetConnectionMgr
{
public:
	NetConnectionMgr( ) {};
	~NetConnectionMgr() {};

	NetConnection * CreateNetConnection();
	void RealseConnection(NetConnection *connection);

    inline void SetKernel(core::IKernel *kernel){ _kernel = kernel; };
protected:
private:
	ConnectionMap	_indexMap;
	s32				_connectionId;
    core::IKernel         *_kernel;
};
#endif