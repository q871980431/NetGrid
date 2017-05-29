#include "NetEngine.h"

#ifdef WIN32
#include <winsock2.h>
#endif

NetEngine::EventBase * NetEngine::s_eventBase = nullptr;
NetEngine::EventListener * NetEngine::s_eventListener = nullptr;
NetEngine::NetConfig NetEngine::s_config;
NetConnectionMgr    NetEngine::s_connectionMgr;

bool NetEngine::Ready()
{
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(2, 2);

    (void)WSAStartup(wVersionRequested, &wsaData);
#endif
    s_eventBase = event_base_new();
    if (s_eventBase == nullptr)
        return false;

    return true;
}

bool NetEngine::Initialize()
{
    return true;
}

bool NetEngine::Destroy()
{

    return true;
}


void NetEngine::Process(s32 tick)
{

}

void NetEngine::CreateNetSession(const char *ip, s16 port, core::ITcpSession *session)
{
    sockaddr_in stAddr;
    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons(port);
    if (0 == strcmp(ip, "0"))
        stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        stAddr.sin_addr.s_addr = inet_addr(ip);

    NetConnection *connction = s_connectionMgr.CreateNetConnection();
    struct bufferevent* bev = bufferevent_socket_new(s_eventBase, -1, BEV_OPT_CLOSE_ON_FREE);
    
    bufferevent_socket_connect(bev, (struct sockaddr *)&stAddr, sizeof(stAddr));
    bufferevent_setcb(bev, OnReadEvent, NULL, OnErrorEvent, connction);
    bufferevent_enable(bev, EV_READ | EV_PERSIST);

    connction->SetBuffEvent(bev);
    connction->SetSession(session);
}

void NetEngine::CreateNetListener(const char *ip, s16 port, core::ITcpListener *listener)
{
    if (s_eventListener != nullptr)
    {
        ASSERT(false, "error");
        return;
    }
    sockaddr_in stAddr;
    stAddr.sin_family = AF_INET;
    stAddr.sin_port = htons(port);
    if (0 == strcmp(ip, "0"))
        stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        stAddr.sin_addr.s_addr = inet_addr(ip);
    s_eventListener = evconnlistener_new_bind(s_eventBase, OnListener, listener,
                                                LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, 32,
                                                (struct sockaddr*)&stAddr, sizeof(stAddr));
}

void NetEngine::OnListener(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *addr, int socklen, void *arg)
{
    core::ITcpListener *tcpListener = (core::ITcpListener *)arg;
    event_base *base = s_eventBase;

    core::ITcpSession   *session = tcpListener->CreateSession();
    NetConnection *connction = s_connectionMgr.CreateNetConnection();

    struct bufferevent* bev;
    evutil_make_socket_nonblocking(fd);
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, OnReadEvent, NULL, OnErrorEvent, connction);
    bufferevent_setwatermark(bev, EV_READ, 0, s_config.revBuffSize);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    session->SetConnection(connction);
    session->OnEstablish();
}

void NetEngine::OnCreateSession( EventBase *eventBase, core::ITcpSession *session, evutil_socket_t fd)
{

}

void NetEngine::OnReadEvent(struct bufferevent* bev, void * ctx)
{
    char msg[1024];

    NetConnection *connetion = (NetConnection *)ctx;
    size_t len = bufferevent_read(bev, msg, sizeof(msg));
    msg[len] = 0;

    printf("\nrecv %s from server", msg);
}

void NetEngine::OnErrorEvent(struct bufferevent* bev, short error, void * ctx)
{
    NetConnection *connetion = (NetConnection *)ctx;
    core::ITcpSession *session = connetion->GetSession();

    if (error & BEV_EVENT_EOF)
    {
        /* connection has been closed, do any clean up here*/
        ASSERT(session != nullptr, "error");
        session->OnTerminate();
    }
    else if (error & BEV_EVENT_CONNECTED)
    {
        ASSERT(session != nullptr, "error");
        session->OnEstablish();
        return;
    }
    else if (error & BEV_EVENT_ERROR)
    {
        /* check errno to see what error occurred*/
    }
    else if (error & BEV_EVENT_TIMEOUT)
    {
        /* must be a timeout event handle, handle it*/
    }
    if (nullptr != session)
        session->SetConnection(nullptr);

    s_connectionMgr.RealseConnection(connetion);
    bufferevent_free(bev);
}