/*
 * File:	Proxy.h
 * Author:	xuping
 * 
 * Created On 2017/8/21 21:28:37
 */

#ifndef __Proxy_h__
#define __Proxy_h__
#include "IProxyMgr.h"
class Proxy : public IProxy, public core::IMsgSession
{
public:
    Proxy(s32 id, IProxyListener *listener):_id(id), _connection(nullptr), _listener(listener), _connected(false){};
    virtual ~Proxy(){};

    virtual void  SetConnection(IMsgConnection *connection){ _connection = connection; };
    virtual void  OnEstablish(){ _connected = true; };
    virtual void  OnTerminate(){ _connected = false; };
    virtual void  OnError(s32 moduleErr, s32 sysErr){};
    virtual void  OnRecv(s32 messageId, const char *buff, s32 len){ _listener->OnRecv(messageId, buff, len); };
    virtual void  SendMsg(s32 messageId, const char *buff, s32 len){ _connection->Send(messageId, buff, len); };
    virtual bool  IsConnect(){ return _connected; };
    virtual void  Close(){ _connection->Close(); _connected = false; _connection = nullptr; };
    inline s32 GetId(){ return _id; };
protected:
private:
    IMsgConnection  *_connection;
    IProxyListener  *_listener;
    bool             _connected;
    s32              _id;
};
#endif