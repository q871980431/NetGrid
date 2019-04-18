#include "NetEngine.h"
#include "../Kernel.h"
#ifdef WIN32
#include <winsock2.h>
#endif
#ifdef LINUX
#include<netinet/in.h>
#include<sys/socket.h>
#include <arpa/inet.h>
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
    core::IKernel *kernel = KERNEL;
    s_connectionMgr.SetKernel(kernel);
    return true;
}

bool NetEngine::Destroy()
{

    return true;
}


void NetEngine::Process(s32 tick)
{
    int rt = event_base_loop(s_eventBase, EVLOOP_NONBLOCK);
}

void NetEngine::CreateNetSession(const char *ip, s16 port, core::IMsgSession *session)
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
    connction->SetBuffEvent(bev);

    bufferevent_setcb(bev, OnReadEvent, OnWriteEvent, OnErrorEvent, connction);
    bufferevent_enable(bev, EV_READ | EV_PERSIST | EV_WRITE);

    connction->SetSession(session);
    connction->SetRomoteAddr(&stAddr);
    session->SetConnection(connction);
    //session->OnEstablish();
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

    core::IMsgSession   *session = tcpListener->CreateSession();
    NetConnection *connction = s_connectionMgr.CreateNetConnection();

    struct bufferevent* bev;
    evutil_make_socket_nonblocking(fd);
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    connction->SetBuffEvent(bev);

    bufferevent_setcb(bev, OnReadEvent, OnWriteEvent, OnErrorEvent, connction);
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    connction->SetRomoteAddr((struct sockaddr_in *)addr);
    connction->SetSession(session);
    session->SetConnection(connction);
    session->OnEstablish();
}

void NetEngine::OnCreateSession( EventBase *eventBase, core::IMsgSession *session, evutil_socket_t fd)
{

}

void NetEngine::OnReadEvent(struct bufferevent* bev, void * ctx)
{
    NetConnection *connetion = (NetConnection *)ctx;
    struct evbuffer *eb = bufferevent_get_input(bev);
    s32 len = evbuffer_get_length(eb);
    while (len > sizeof(core::MessageHead))
    {
        core::MessageHead head;
        evbuffer_copyout(eb, &head, sizeof(core::MessageHead));
        if (head.len > len)
            break;
        s32 continuous = evbuffer_get_contiguous_space(eb);
        void *buff = nullptr;
        if (continuous > head.len)
            buff =  evbuffer_pullup(eb, head.len);
        else
        {
            buff = alloca(head.len);
            evbuffer_copyout(eb, buff, head.len);
        }
        connetion->OnReceive(head.messageId, (char *)buff + sizeof(core::MessageHead), head.len - sizeof(core::MessageHead));
        evbuffer_drain(eb, head.len);
        len -= head.len;
    }
}

void NetEngine::OnWriteEvent(struct bufferevent* bev, void * ctx)
{
    NetConnection *connetion = (NetConnection *)ctx;
    connetion->OnSend();
}

void NetEngine::OnErrorEvent(struct bufferevent* bev, short error, void * ctx)
{
    NetConnection *connetion = (NetConnection *)ctx;
    core::IMsgSession *session = connetion->GetSession();

    if (error & BEV_EVENT_EOF)
    {
        /* connection has been closed, do any clean up here*/
        ASSERT(session != nullptr, "error");
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

	session->SetConnection(nullptr);
	session->OnTerminate();
    s_connectionMgr.RealseConnection(connetion);
    bufferevent_free(bev);
}