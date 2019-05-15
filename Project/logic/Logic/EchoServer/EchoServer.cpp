/*
 * File:	EchoServer.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/10 15:52:57
 */

#include "EchoServer.h"
#include "XmlReader.h"
#include "Game_tools.h"
#include <vector>

EchoServer * EchoServer::s_self = nullptr;
IKernel * EchoServer::s_kernel = nullptr;
s64 EchoServer::s_sessionId = 0;
EchoServer::ConfigInfo EchoServer::s_configInfo;

bool EchoServer::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	LoadConfigFile();
    return true;
}

bool EchoServer::Launched(IKernel *kernel)
{
	if (s_configInfo.isServer)
		StartServer();
	else
		StartClient(kernel);

    return true;
}

bool EchoServer::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


ITcpSession * EchoServer::CreateSession()
{
	EchoSession *session = NEW EchoSession(s_kernel, GetNewSessionId());
	return session;
}

void EchoServer::StartServer()
{
	IKernel *kernel = s_kernel;
	TRACE_LOG("StartServer:%s, %d", s_configInfo.ip.GetString(), s_configInfo.port);
	s_kernel->CreateNetListener(s_configInfo.ip.GetString(), s_configInfo.port, this);
}

void EchoServer::StartClient(core::IKernel *kernel)
{
	auto onTimeFun = [](core::IKernel *kernel, s64 tick)
	{
		EchoClientSession *client = NEW EchoClientSession(s_kernel, s_self->GetNewSessionId());
		s_kernel->CreateNetSession(s_configInfo.ip.GetString(), s_configInfo.port, client);
	};

	SimpleTimer *timer = NEW SimpleTimer(nullptr, onTimeFun, nullptr);
	ADD_TIMER(timer, 10, s_configInfo.clientNum, 2000);
}

bool EchoServer::LoadConfigFile()
{
	IKernel *kernel = s_kernel;
	char buff[MAX_PATH] = { 0 };
	SafeSprintf(buff, sizeof(buff), "%s/config.xml", s_kernel->GetEnvirPath());
	XmlReader reader;
	if (!reader.LoadFile(buff))
	{
		ASSERT(false, "error");
		return false;
	}
	IXmlObject *echoServer = reader.Root()->GetFirstChrild("echoserver");
	if (echoServer)
	{
		s_configInfo.isServer = echoServer->GetAttribute_Bool("server");
		s_configInfo.ip = echoServer->GetAttribute_Str("ip");
		s_configInfo.port = echoServer->GetAttribute_S32("port");
		s_configInfo.clientNum = echoServer->GetAttribute_S32("clientNum");
	}
	TRACE_LOG("Config Server:%d, ip:%s, port:%d, clientNum:%d", s_configInfo.isServer, s_configInfo.ip, s_configInfo.port, s_configInfo.clientNum);

	return true;
}