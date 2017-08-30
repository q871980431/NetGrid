/*
 * File:	IProxyMgr.h
 * Author:	xuping
 * 
 * Created On 2017/8/21 21:28:37
 */

#ifndef  __IProxyMgr_h__
#define __IProxyMgr_h__
#include "IModule.h"
#include <functional>
#include "IStream.h"
class IProxyListener
{
public:
    virtual ~IProxyListener(){};

    virtual void OnOpen() = 0;
    virtual void OnClose() = 0;
    virtual void OnRecv(s32 messageId, const char *buff, s32 len) = 0;
};

class IProxy
{
public:
    virtual ~IProxy(){};
    virtual void SendMsg(s32 messageId, const char *buff, s32 len) = 0;
    virtual bool IsConnect() = 0;
    virtual void Close() = 0;
};

class IProxyServiceListener
{
public:
    virtual ~IProxyServiceListener(){};
    virtual void OnProxyConnect(s32 proxyId) = 0;
    virtual void OnProxyDisconnect(s32 proxyId) = 0;
    virtual void OnRecvProxyMsg(s32 proxyId, s32 messageId, const char *buff, s32 len) = 0;
};

class IProxyService
{
public:
    virtual void SendMsg(s32 proxyId, s32 messageId, const char *buff, s32 len) = 0;
    virtual void KickProxy(s32 proxyId) = 0;
    virtual void Stop() = 0;
};

class IProxyMgr : public IModule
{
public:
    virtual ~IProxyMgr(){};
    virtual IProxy * CreateProxy(IProxyListener *listener, const char *ip, s16 port) = 0;
    virtual void RecoverProxy(IProxy *proxy) = 0;
    virtual IProxyService * CreateProxyService(IProxyServiceListener *listener, const char *ip, s16 port) = 0;
    virtual void RecoverProxyService(IProxyService *proxyService) = 0;
};
#endif