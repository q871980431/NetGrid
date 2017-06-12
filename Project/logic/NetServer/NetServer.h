/*
 * File:	NetServer.h
 * Author:	xuping
 * 
 * Created On 2017/5/29 21:06:25
 */

#ifndef __NetServer_h__
#define __NetServer_h__
#include "INetServer.h"
class GameSession : public core::IMsgSession
{
    virtual void  SetConnection(core::IMsgConnection *connection){ _connection = connection; };
    virtual void  OnEstablish(){
        if (_connection)
        {
            s32 messageId = 1;
            char *msg = "HelloClient";
            _connection->Send(messageId, msg, strlen(msg) + 1);
        }
    };
    virtual void  OnTerminate(){};
    virtual void  OnError(s32 moduleErr, s32 sysErr){};
    virtual void  OnRecv(s32 messageId, const char *buff, s32 len);
private:
    core::IMsgConnection *_connection;
	s32					  _count;
	s64					  _first;
};

class NetListener : public core::ITcpListener
{
public:
    virtual IMsgSession * CreateSession(){ return NEW GameSession(); };
};

class NetServer : public INetServer
{
public:
    virtual ~NetServer(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static NetServer     * s_self;
    static IKernel  * s_kernel;
    static NetListener  s_netListener;
    static GameSession s_session;
};
#endif