#include "ProxyService.h"
#include "Proxy.h"
IMsgSession * ProxyService::CreateSession()
{
    ProxySession *session = CREAT_FROM_POOL(_pool, this, ++_id);
    _sessionMap.insert(std::make_pair(_id, session));
    return session;
}

void ProxyService::SendMsg(s32 proxyId, s32 messageId, const char *buff, s32 len)
{
    auto iter = _sessionMap.find(proxyId);
    ASSERT(iter != _sessionMap.end(), "error");
    if (iter != _sessionMap.end())
        iter->second->SendMsg(messageId, buff, len);
}

void ProxyService::KickProxy(s32 proxyId)
{
    auto iter = _sessionMap.find(proxyId);
    ASSERT(iter != _sessionMap.end(), "error");
    if (iter != _sessionMap.end())
    {
        iter->second->Close();
        _pool.Recover(iter->second);
        _sessionMap.erase(iter);
    }
}

void ProxyService::Stop()
{
    auto iter = _sessionMap.begin();
    auto end = _sessionMap.end();
    for (; iter != end; iter++)
    {
        iter->second->Close();
        _pool.Recover(iter->second);
    }
    _sessionMap.clear();

}