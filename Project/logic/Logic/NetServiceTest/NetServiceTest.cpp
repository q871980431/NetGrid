/*
 * File:	NetServiceTest.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/30 16:44:29
 */

#include "NetServiceTest.h"
#include "XmlReader.h"
#include "Game_tools.h"
#include "Tools_Net.h"
#include "CircularBuffer.h"

NetServiceTest * NetServiceTest::s_self = nullptr;
IKernel * NetServiceTest::s_kernel = nullptr;
INetService * NetServiceTest::s_netService = nullptr;
NetServiceTest::ConfigInfo NetServiceTest::s_configInfo;

bool NetServiceTest::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	LoadConfigFile();

    return true;
}

bool NetServiceTest::Launched(IKernel *kernel)
{
	FIND_MODULE(s_netService, NetService);
	if (s_configInfo.isServer)
		StartServer();
	else
		StartClient(kernel);

    return true;
}

bool NetServiceTest::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

ITcpSession * NetServiceTest::CreateSession()
{
	EchoSession *session = NEW EchoSession(s_kernel);
	return session;
}

void NetServiceTest::StartServer()
{
	IKernel *kernel = s_kernel;
	TRACE_LOG("StartServer:%s, %d", s_configInfo.ip.GetString(), s_configInfo.port);
	s_netService->CreateNetListener(s_configInfo.ip.GetString(), s_configInfo.port, this);
}

void NetServiceTest::StartClient(core::IKernel *kernel)
{
	auto onTimeFun = [](core::IKernel *kernel, s64 tick)
	{
		EchoClientSession *client = NEW EchoClientSession(s_kernel, s_configInfo.clientClose);
		s_netService->CreateNetConnecter(s_configInfo.ip.GetString(), s_configInfo.port, client);
	};

	SimpleTimer *timer = NEW SimpleTimer(nullptr, onTimeFun, nullptr);
	ADD_TIMER(timer, 10, s_configInfo.clientNum, 2000);
}

bool NetServiceTest::LoadConfigFile()
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
	IXmlObject *echoServer = reader.Root()->GetFirstChrild("servicetest");
	if (echoServer)
	{
		s_configInfo.isServer = echoServer->GetAttribute_Bool("server");
		s_configInfo.ip = echoServer->GetAttribute_Str("ip");
		s_configInfo.port = echoServer->GetAttribute_S32("port");
		s_configInfo.clientNum = echoServer->GetAttribute_S32("clientNum");
		s_configInfo.clientClose = echoServer->GetAttribute_Bool("clientclose");
	}
	TRACE_LOG("Config Server:%d, ip:%s, port:%d, clientNum:%d, clientclose:%d", s_configInfo.isServer, s_configInfo.ip, s_configInfo.port, s_configInfo.clientNum, s_configInfo.clientClose);

	return true;
}