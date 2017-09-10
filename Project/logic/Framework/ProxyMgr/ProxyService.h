/*
 * File:	ProxyMgr.h
 * Author:	xuping
 * 
 * Created On 2017/8/21 21:28:37
 */

#ifndef __ProxyService_h__
#define __ProxyService_h__
#include "IProxyMgr.h"
#include "TDynPool.h"
#include <unordered_map>
class ProxyService;
class ProxySession : public core::IMsgSession
{
public:
	ProxySession(ProxyService *service, s32 id) :_service(service), _id(id), _connection(nullptr), _connected(false) {};
	virtual ~ProxySession() {};
	virtual void  SetConnection(IMsgConnection *connection) { _connection = connection; };
	virtual void  OnEstablish();
	virtual void  OnTerminate();
	virtual void  OnError(s32 moduleErr, s32 sysErr) {};
	virtual void  OnRecv(s32 messageId, const char *buff, s32 len);
public:
	inline bool  SendMsg(s32 messageId, const char *buff, s32 len) { _connection->Send(messageId, buff, len); return true; };
	inline bool  IsConnect() { return _connected; };
	inline void  Close() { _connection->Close(); _connected = false; _connection = nullptr; };
private:
	IMsgConnection  *_connection;
	bool             _connected;
	s32              _id;
	ProxyService    *_service;
};
typedef tlib::TDynPool<ProxySession> ProxySessionPool;
typedef std::unordered_map<s32, ProxySession *> ProxySessionMap;
class ProxyService : public IProxyService, public core::ITcpListener
{
public:
    ProxyService(s32 id, IProxyServiceListener *listener):_listener(listener), _id(id){};
    virtual ~ProxyService(){};

    virtual IMsgSession * CreateSession();
    virtual void SendMsg(s32 proxyId, s32 messageId, const char *buff, s32 len);
    virtual void KickProxy(s32 proxyId);
    virtual void Stop();
public:
    inline void OnOpen(s32 proxyId){ _listener->OnProxyConnect(proxyId); };
    inline void OnClose(s32 proxyId){ _listener->OnProxyDisconnect(proxyId); };
    inline void OnRecv(s32 proxyId, s32 messageId, const char *buff, s32 len){ _listener->OnRecvProxyMsg(proxyId, messageId, buff, len); };
    inline s32  GetId(){ return _id; };
protected:
private:
    IProxyServiceListener *_listener;
    s32     _id;
    ProxySessionPool       _pool;
    ProxySessionMap        _sessionMap;
};


#endif