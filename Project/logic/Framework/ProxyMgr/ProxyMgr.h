/*
 * File:	ProxyMgr.h
 * Author:	xuping
 * 
 * Created On 2017/8/21 21:28:37
 */

#ifndef __ProxyMgr_h__
#define __ProxyMgr_h__
#include "IProxyMgr.h"
#include <unordered_map>
typedef std::unordered_map<s32, IProxy *> ProxyMap;
typedef std::unordered_map<s32, IProxyService *> ProxyServiceMap;
class ProxyMgr : public IProxyMgr
{
public:
    virtual ~ProxyMgr(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
    virtual IProxy * CreateProxy(IProxyListener *listener, const char *ip, s16 port);
    virtual void RecoverProxy(IProxy *proxy);
    virtual IProxyService * CreateProxyService(IProxyServiceListener *listener, const char *ip, s16 port);
    virtual void RecoverProxyService(IProxyService *proxyService);
protected:
private:
    inline s32 NextID(){ return ++s_proxyID; };
private:
    static ProxyMgr     * s_self;
    static IKernel  * s_kernel;
    static ProxyMap             s_proxyMap;
    static ProxyServiceMap      s_proxyServiceMap;
    static s32                  s_proxyID;
};
#endif