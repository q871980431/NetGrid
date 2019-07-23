/*
 * File:	NetService.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#include "NetIOEngine.h"
#include "IOEngine_Epoll.h"
#include "IOEngine_IOCP.h"


IIODriver::IIODriver(TcpConnection *connection): _connetion(connection), _errorCode(0)
{
	_connetion->SetIODriver(this);
	_sendBuff = NEW CircluarBuffer(_connetion->GetSendBuffSize());
	_recvBuff = NEW CircluarBuffer(_connetion->GetRecvBuffSize());
	_close = false;
	_recvFin = false;
	_activeClose = false;
}

IIODriver::~IIODriver()
{
	_connetion->SetIODriver(nullptr);
	DEL _sendBuff;
	DEL _recvBuff;
}

NetIOEngine::NetIOEngine(s32 engineNum)
{
	_engineNum = engineNum;
}

bool NetIOEngine::Init(IKernel *kernel)
{
#ifdef LINUX
	for (s32 i = 0; i < _engineNum; i++)
	{
		IIOEngine *engine = NEW IOEngineEpoll(1024);
		engine->SetId(i+1);
		engine->Init(kernel);
		_ioengines.push_back(engine);
	}
#endif

#ifdef WIN32
	IIOEngine *engine = NEW IOEngineIocp(_engineNum);
	engine->SetId(1);
	engine->Init(kernel);
	_ioengines.push_back(engine);
#endif

	return true;
}

bool NetIOEngine::AddConnection(TcpConnection *connection)
{
	if (connection->GetIOEngine() == nullptr)
	{
		IIOEngine *ioEngine = GetExecIOEngine();
		connection->SetIOEngine(ioEngine);

		ioEngine->CreateDriver(connection);
		return true;
	}
	return false;
}

bool NetIOEngine::RemoveConnection(TcpConnection *connection)
{
	IIOEngine *ioEngine = connection->GetIOEngine();
	if ( ioEngine != nullptr)
	{
		ioEngine->RemoveConnection(connection);
		connection->SetIOEngine(nullptr);
		return true;
	}
	return false;
}

void NetIOEngine::Process(IKernel *kernel, s64 tick)
{
	for (auto iter : _ioengines)
	{
		iter->Process(kernel, tick);
	}
}

IIOEngine * NetIOEngine::GetExecIOEngine()
{
#ifdef WIN32
	return _ioengines[0];
#endif

#ifdef LINUX
	s32 index = 0;
	s32 count = 0;
	for (s32 i = 0; i < _engineNum; i++)
	{
		if (i == 0)
			count = _ioengines[i]->GetDriverCount();
		else
		{
			s32 tmp = _ioengines[i]->GetDriverCount();
			if (tmp < count)
				count = tmp;
		}
	}

	return _ioengines[index];
#endif
}
