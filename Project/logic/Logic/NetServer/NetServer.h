/*
 * File:	NetServer.h
 * Author:	xuping
 * 
 * Created On 2017/5/29 21:06:25
 */

#ifndef __NetServer_h__
#define __NetServer_h__
#include "INetServer.h"
class GameSession : public core::ITcpSession
{
public:
	virtual ~GameSession() {};

    virtual void  SetConnection(core::ITcpConnection *connection){ _connection = connection; };
    virtual void  OnEstablish(){
        if (_connection)
        {
            const char *ip = _connection->GetRemoteIP();
            ECHO("\n*************OOnEstablish IP=%s******************", ip);
            s32 messageId = 1;
            const char *msg = "HelloClient";
            SendMsg(messageId, msg, (s32)strlen(msg) + 1);
        }
    };
    virtual void  OnTerminate(){};
    virtual void  OnError(s32 moduleErr, s32 sysErr){};
	virtual void  OnRecv(const char *buff, s32 len);
	virtual s32	  OnParsePacket(CircluarBuffer *recvBuff);
    virtual void  OnRecv(s32 messageId, const char *buff, s32 len);
	virtual void  OnRelease() { DEL this; };
	void SendMsg(s32 msgId, const char *buff, s32 len);
private:
    core::ITcpConnection *_connection;
	s32					  _count;
	s64					  _first;
};

class NetListener : public core::INetTcpListener
{
public:
    virtual ITcpSession * CreateSession(){ return NEW GameSession(); };
	virtual void OnRelease() {};
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