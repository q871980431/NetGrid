/*
 * File:	ProxyMgr.cpp
 * Author:	xuping
 * 
 * Created On 2017/8/21 21:28:37
 */

#include "ProxyMgr.h"
#include "Proxy.h"
#include "ProxyService.h"

ProxyMgr * ProxyMgr::s_self = nullptr;
IKernel * ProxyMgr::s_kernel = nullptr;
ProxyMap    ProxyMgr::s_proxyMap;
ProxyServiceMap ProxyMgr::s_proxyServiceMap;
s32         ProxyMgr::s_proxyID = 0;
bool ProxyMgr::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool ProxyMgr::Launched(IKernel *kernel)
{

    return true;
}

bool ProxyMgr::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}
IProxy * ProxyMgr::CreateProxy(IProxyListener *listener, const char *ip, s16 port)
{
    if (listener != nullptr)
    {
        s32 id = NextID();
        Proxy *proxy = NEW Proxy(id, listener);
        s_kernel->CreateNetSession(ip, port, proxy);
        auto rt = s_proxyMap.insert(std::make_pair(id, proxy));
        ASSERT(rt.second, "error");

        return proxy;
    }
    return nullptr;
}

void ProxyMgr::RecoverProxy(IProxy *proxy)
{
    s32 id = ((Proxy *)proxy)->GetId();
    auto iter = s_proxyMap.find(id);
    ASSERT(iter != s_proxyMap.end(), "error");
    if (iter != s_proxyMap.end())
    {
        if (proxy->IsConnect())
            proxy->Close();
        DEL iter->second;
        s_proxyMap.erase(iter);
    }
}

IProxyService * ProxyMgr::CreateProxyService(IProxyServiceListener *listener, const char *ip, s16 port)
{
    if (listener != nullptr)
    {
        s32 id = NextID();
        ProxyService *proxyService = NEW ProxyService(id, listener);
        s_kernel->CreateNetListener(ip, port, proxyService);
        auto rt = s_proxyServiceMap.insert(std::make_pair(id, proxyService));
        ASSERT(rt.second, "error");

        return proxyService;
    }

    return nullptr;
}

void ProxyMgr::RecoverProxyService(IProxyService *proxyService)
{
    s32 id = ((Proxy *)proxyService)->GetId();
    auto iter = s_proxyServiceMap.find(id);
    ASSERT(iter != s_proxyServiceMap.end(), "error");
    if (iter != s_proxyServiceMap.end())
    {
        proxyService->Stop();
        DEL iter->second;
        s_proxyServiceMap.erase(iter);
    }
}
