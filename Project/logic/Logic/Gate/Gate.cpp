/*
 * File:	Gate.cpp
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:15:22
 */

#include "Gate.h"
#include "XmlReader.h"
#include "ClientMgr.h"

Gate * Gate::s_self = nullptr;
IKernel * Gate::s_kernel = nullptr;
GateConfigInfo Gate::s_configInfo;

bool Gate::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;

	if (!LoadConfig())
		return false;

    return true;
}

bool Gate::Launched(IKernel *kernel)
{
	s_kernel->CreateNetListener(s_configInfo.hostIp.c_str(), s_configInfo.port, &CLIENTMGR);

    return true;
}

bool Gate::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


void Gate::OnClientEnter(Client *client)
{
	TRACE_LOG("client index:%d enter", client->GetIndex());
}

void Gate::OnClientMsg(Client *client, const char *content, s32 len)
{
	TRACE_LOG("client index:%d recv msg len:%d", client->GetIndex(), len);
}


void Gate::OnClientLeave(Client *client)
{
	TRACE_LOG("client index:%d leave", client->GetIndex());
}

bool Gate::LoadConfig()
{
	XmlReader reader;
	if (!reader.LoadFile(s_kernel->GetConfigFile()))
		return false;
	IXmlObject *gatObj = reader.Root()->GetFirstChrild("gate");
	if (gatObj == nullptr)
	{
		DEBUG_LOG("don't find gate config");
		return false;
	}
	s_configInfo.hostIp = gatObj->GetAttribute_Str("ip");
	s_configInfo.port = gatObj->GetAttribute_S32("port");
	s_configInfo.heartBeat = gatObj->GetAttribute_S32("heartbeat");

	return true;
}
