/*
 * File:	NetService.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */
#include "IOEngine_IOCP.h"
#ifdef WIN32
#include "Tools_Net.h"
#include "Tools.h"
#include <signal.h>

IocpDriver::IocpDriver(TcpConnection *connection, HANDLE complatPort):IIODriver(connection)
{
	_complationPort = complatPort;
	tools::Zero(_sendPerData);
	tools::Zero(_recvPerData);
	_sendPerData.opType = SEND;
	_recvPerData.opType = RECV;
	_sending = false;
	_recving = false;
	_nextRecvBuff = nullptr;
}

bool IocpDriver::SettingBuffSize(s32 recvSize, s32 sendSize)
{
	if (_nextRecvBuff != nullptr)
		return false;

	{
		std::lock_guard<std::mutex> lock(_sendMutex);
		if (_sending == true)
			return false;
	}

	CircluarBuffer *tempSendBuff = NEW CircluarBuffer(recvSize);
	std::swap(_sendBuff, tempSendBuff);
	DEL tempSendBuff;

	_nextRecvBuff = NEW CircluarBuffer(sendSize);

	return true;
}

void IocpDriver::OnError(IKernel *kernel, OVERLAPPED *perIoOperation)
{
	THREAD_LOG("Error", "Socket ErrorCode:%d", tools::GetSocketError());
	switch (((PerIoOperationData *)perIoOperation)->opType)
	{
	case SEND: {_sending = false; EnterClose(); };
	case RECV: {_recving = false; EnterClose(); };
	default:
		break;
	}
}

void IocpDriver::OnEvent(IKernel *kernel, OVERLAPPED *perIoOperation, s32 transferSize)
{
	switch (((PerIoOperationData *)perIoOperation)->opType)
	{
	case SEND: {OnSend(transferSize); return; };
	case RECV: {OnRead(kernel, transferSize); return; };
	default:
		break;
	}
	THREAD_LOG("Event Opt Type Error, Type:%d", ((PerIoOperationData *)perIoOperation)->opType );
}

bool IocpDriver::OnSend(s32 transferSize)
{
	_sending = false;
	if (transferSize == 0)
	{
		_recvFin = true;
		EnterClose();
		return false;
	}
	_sendBuff->Read(transferSize);
	if (_sendBuff->DataSize() == 0)
	{
		if (_activeClose)
		{
			shutdown(GetNetSocket(), SD_SEND);
			return false;
		}
	}

	PostSend();
	return true;
}

bool IocpDriver::OnRead(IKernel *kernel, s32 transferSize)
{
	_recving = false;
	if (transferSize == 0)
	{
		_recvFin = true;
		EnterClose();
		return false;
	}
	THREAD_LOG("NetWork", "receive data size:%d", transferSize);
	_recvBuff->WriteBuff(transferSize);
	PostRecv(kernel);
	return true;
}

bool IocpDriver::PostSend()
{
	{
		std::lock_guard<std::mutex> lock(_sendMutex);
		if (_sending == true)
			return true;
		_sending = true;
	}

	s32 len = 0;
	_sendPerData.dataBuff.buf = _sendBuff->GetCanReadBuff(len);
	if (len == 0)
	{
		_sending = false;
		return true;
	}
	_sendPerData.dataBuff.len = len;
	DWORD sendBytes;
	DWORD flags = 0;
	if (WSASend(GetNetSocket(), &_sendPerData.dataBuff, 1, &sendBytes, flags, &_sendPerData.overlapped, nullptr) == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING == tools::GetSocketError())
			return true;
		_sending = false;
		EnterClose();
		return false;
	}
	return true;
}

bool IocpDriver::PostRecv(IKernel *kernel)
{
	if (_nextRecvBuff)
	{
		s32 canWritLen = 0;
		char *canWritBuff = _nextRecvBuff->GetCanWriteBuff(canWritLen);
		s32 readLen = _recvBuff->Read(canWritBuff, canWritLen);
		_nextRecvBuff->WriteBuff(readLen);
		DEL _recvBuff;
		_recvBuff = _nextRecvBuff;
		_nextRecvBuff = nullptr;
	}

	ASSERT(_recving == false, "error");
	s32 len = 0;
	_recvPerData.dataBuff.buf = _recvBuff->GetCanWriteBuff(len);
	if (len == 0)
	{
		_close = true;
		return false;
	}
	_recvPerData.dataBuff.len = len;
	DWORD recvBytes;
	DWORD flags = 0;
	_recving = true;
	if (WSARecv(GetNetSocket(), &_recvPerData.dataBuff, 1, &recvBytes, &flags, &_recvPerData.overlapped, nullptr) == SOCKET_ERROR)
	{
		if (WSA_IO_PENDING == tools::GetSocketError())
			return true;

		_recving = false;
		EnterClose();
		return false;
	}	
	return true;
}

void IocpDriver::BindCompletionPort()
{
	CreateIoCompletionPort((HANDLE)GetNetSocket(), _complationPort, (ULONG_PTR)this, 0);
}

bool IocpDriver::CheckClose()
{
	if (_close && !_sending && !_recving)
	{
		OnClose();
		return true;
	}
	return false;
}

void IocpDriver::EnterClose()
{
	if (!_close)
	{
		_close = true;
		_errorCode = tools::GetSocketError();
		IOEngineIocp *engine = dynamic_cast<IOEngineIocp*>(_connetion->GetIOEngine());
		ASSERT(engine != nullptr, "error");
		if (engine)
		{
			engine->EnterDel(this);
		}
	}
}

IOEngineIocp::IOEngineIocp(s32 size):_mainQueue(QUEUE_SIZE), _threadQueue(QUEUE_SIZE)
{
	_threadSize = size;
}

bool IOEngineIocp::Init(IKernel *kernel)
{
	//signal(SIGPIPE, SIG_IGN);
	_kernel = kernel;
	_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, _threadSize);
	if (_completionPort == NULL)
	{
		TRACE_LOG("Epoll Init error, error code:%d", tools::GetSocketError());
		return false;
	}
	//根据系统的CPU来创建工作者线程
	//SYSTEM_INFO SystemInfo;
	//GetSystemInfo(&SystemInfo);
	for (int i = 0; i < _threadSize; i++)
	{
		_workThread.push_back(std::thread(&IOEngineIocp::Run, this));
	}
	_terminate = false;
	TRACE_LOG("Epoll Engine Init Success");
	return true;
}

bool IOEngineIocp::Stop(IKernel *kernel)
{
	return true;
}

void IOEngineIocp::Process(IKernel *kernel, s64 tick)
{
	std::vector<IocpDriver *> temps;
	{
		std::lock_guard<std::mutex> lock(_delMutex);
		temps.swap(_dels);
	}
	for (auto iter : temps)
	{
		auto tempIter = _readyDelDriver.find(iter->GetNetSocket());
		if (tempIter != _readyDelDriver.end())
			_readyDelDriver.erase(tempIter);

		_hasDel.insert(std::make_pair(iter->GetNetSocket(), iter));
	}

	for (auto iter = _hasDel.begin(); iter != _hasDel.end(); )
	{
		if (!iter->second->CheckClose())
		{
			iter++;
			continue;
		}
		iter = _hasDel.erase(iter);
	}

	for (auto &iter : _driversMain)
	{
		if (!iter.second->Sending())
			iter.second->PostSend();
	}

	for (auto iter = _readyDelDriver.begin(); iter != _readyDelDriver.end(); )
	{
		if (tick - iter->second.tick >= IIOEngine::DELAY_SEND * 1000)
		{
			shutdown(iter->second.dirver->GetNetSocket(), SD_BOTH);
			iter = _readyDelDriver.erase(iter);
			continue;
		}
		iter++;
	}
}

bool IOEngineIocp::AddIODriver(IIODriver *ioDriver)
{
	IKernel *kernel = _kernel;
	TRACE_LOG("Engine:%d, Add IO Driver Event1", GetId());
	IocpDriver *driver = dynamic_cast<IocpDriver*>(ioDriver);
	ASSERT(driver != nullptr, "error");
	if (nullptr != driver)
	{
		driver->BindCompletionPort();
		driver->PostRecv(_kernel);
		driver->OnEstablish();
		return true;
	}
	return false;
}

bool IOEngineIocp::RemoveIODriver(IIODriver *ioDriver)
{
	IKernel *kernel = _kernel;
	TRACE_LOG("Engine:%d, Remove IO Driver", GetId());
	IocpDriver *driver = dynamic_cast<IocpDriver*>(ioDriver);
	ASSERT(driver != nullptr, "error");
	if (nullptr != driver)
	{
		if (driver->GetSendBuff()->DataSize() == 0)
		{
			shutdown(driver->GetNetSocket(), SD_BOTH);
		}
		else
		{
			driver->ActiveClose();
		}
		return true;
	}
	return false;
}

IIODriver * IOEngineIocp::CreateDriver(TcpConnection *connection)
{
	auto iter = _driversMain.find(connection->GetSessionId());
	if (iter == _driversMain.end())
	{
		IocpDriver *driver = NEW IocpDriver(connection, _completionPort);
		AddIODriver(driver);
		IKernel *kernel = _kernel;
		TRACE_LOG("Insert drivermain, sessionId:%d", connection->GetSessionId());
		_driversMain.emplace(connection->GetSessionId(), driver);
		++_driverCount;

		return driver;
	}

	return nullptr;
}

void IOEngineIocp::RemoveConnection(TcpConnection *connection)
{
	IKernel *kernel = _kernel;
	auto iter = _driversMain.find(connection->GetSessionId());
	ASSERT(iter != _driversMain.end(), "error");
	if (iter != _driversMain.end())
	{
		ASSERT(iter->second->HasClose(), "error");
		if (iter->second->HasClose())
		{
			DEL iter->second;
			_driversMain.erase(iter);
			TRACE_LOG("del drivermain, sessionId:%d", connection->GetSessionId());
			--_driverCount;
		}
	}
}

void IOEngineIocp::EnterDel(IocpDriver *driver)
{
	std::lock_guard<std::mutex> lock(_delMutex);
	_dels.push_back(driver);
}

void IOEngineIocp::Run()
{
	IKernel *kernel = _kernel;
	DWORD transferSize = 0;
	ULONG_PTR perIoHandle;
	OVERLAPPED *overlapped;
	while (true)
	{
		if (0 == GetQueuedCompletionStatus(_completionPort, &transferSize, &perIoHandle, &overlapped, INFINITE))
		{
			if (overlapped == nullptr)
				continue;
			if (perIoHandle)
				((IocpDriver*)perIoHandle)->OnError(kernel, overlapped);
			else {
				THREAD_LOG("GetQueuedCompletionStatus Error:%d", tools::GetSocketError());
			}
		}
		else
		{
			IocpDriver *driver = (IocpDriver *)perIoHandle;
			driver->OnEvent(kernel, overlapped, transferSize);
		}

		if (_terminate)
			break;
	}
}

#endif

