/*
 * File:	EchoServer.h
 * Author:	xuping
 * 
 * Created On 2019/4/10 15:52:57
 */

#ifndef __EchoServer_h__
#define __EchoServer_h__
#include "IEchoServer.h"
#include "EchoSession.h"
#include "TString.h"

class EchoServer : public IEchoServer, public INetTcpListener
{
	struct ConfigInfo 
	{
		bool isServer;
		tlib::TString<64> ip;
		s32 port;
		s32 clientNum;
	};

public:
    virtual ~EchoServer(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
	virtual ITcpSession * CreateSession();
	virtual void OnRelease() {};

protected:
	inline s64 GetNewSessionId() { return ++s_sessionId; };
	void StartServer();
	void StartClient(core::IKernel *kernel);
	bool LoadConfigFile();
private:
    static EchoServer     * s_self;
    static IKernel  * s_kernel;

	static s64	s_sessionId;
	static bool s_server;
	static ConfigInfo s_configInfo;
};
#endif