#ifndef __NetEngine_h__
#define __NetEngine_h__
#include "INetEngine.h"
#include "MultiSys.h"
#include "IKernel.h"
#include "NetConnection.h"

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h" 
#include "event2/event_struct.h"
#include "event2/listener.h"
#include "event2/util.h"

class NetEngine : public INetEngine
{
    struct NetConfig 
    {
        s32 revBuffSize;
        s32 sendBuffSize;
    };



    typedef struct event_base       EventBase;
    typedef struct evconnlistener   EventListener;
public:
    CREATE_INSTANCE(NetEngine);

    virtual bool Ready();
    virtual bool Initialize();
    virtual bool Destroy();
public:
    virtual void Process(s32 tick);
    virtual void CreateNetSession(const char *ip, s16 port, core::IMsgSession *session);
    virtual void CreateNetListener(const char *ip, s16 port, core::ITcpListener *listener);
private:
    static void OnListener(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg);
    static void OnReadEvent(struct bufferevent* bev, void * ctx);
    static void OnWriteEvent(struct bufferevent* bev, void * ctx);
    static void OnErrorEvent(struct bufferevent* bev, short error, void * ctx);

    
    static void OnCreateSession(EventBase *eventBase, core::IMsgSession *session, evutil_socket_t fd);
private:
    static void LinkSession(core::IMsgConnection *connection, core::IMsgSession *session);

private:
    static EventBase        * s_eventBase;
    static EventListener    * s_eventListener;
    static NetConfig          s_config;
    static NetConnectionMgr   s_connectionMgr;
};

#endif