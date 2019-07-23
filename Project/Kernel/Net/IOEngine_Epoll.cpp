/*
 * File:	NetService.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */
#include "IOEngine_Epoll.h"
#ifdef LINUX
#include "Tools_Net.h"
#include "Tools_time.h"
#include <signal.h>

bool EpollDriver::SettingBuffSize(s32 recvSize, s32 sendSize)
{
	CircluarBuffer *tempSendBuff = NEW CircluarBuffer(recvSize);
	CircluarBuffer *tempRecvBuff = NEW CircluarBuffer(sendSize);
	std::swap(_sendBuff, tempSendBuff);
	std::swap(_recvBuff, tempRecvBuff);
	DEL tempSendBuff;
	DEL tempRecvBuff;

	return true;
}

bool EpollDriver::OnEvent(IKernel *kernel, u32 events)
{
	ASSERT(!_close, "error");
	if (events & (EPOLLERR))
	{
		_errorCode = tools::GetSocketError();
		return false;
	}
	if (events & (EPOLLHUP | EPOLLRDHUP))
	{
		_recvFin = true;
		return false;
	}

	if (events & EPOLLIN)
	{
		return OnRead();
	}
	if (events & EPOLLOUT)
	{
		return OnSend();
	}

	return true;
}

bool EpollDriver::OnSend()
{
	s32 buffLen = 0;
	char *buff = nullptr;
	while (_sendBuff->DataSize() > 0)
	{
		buff = _sendBuff->GetCanReadBuff(buffLen);
		if (buff)
		{
			auto size = send(_connetion->GetNetSocket(), buff, (s32)buffLen, 0);
			if (size == 0)
				return false;
			if (size > 0)
				_sendBuff->Read(size);
			else
			{
				if (tools::GetSocketError() == EAGAIN)
					return true;
				return false;
			}
		}
	}
	if (_activeClose)
	{
		shutdown(_connetion->GetNetSocket(), SHUT_WR);
		return true;
	}

	return true;
}

bool EpollDriver::OnRead()
{
	s32 len = 0;
	char *buff = nullptr;
	while (true)
	{
		buff = _recvBuff->GetCanWriteBuff(len);
		if (buff == nullptr) return false;
		auto size = recv(_connetion->GetNetSocket(), buff, len, 0);
		if (size == 0)
		{
			_recvFin = true;
			return false;
		}
		if (size > 0)
			_recvBuff->WriteBuff((s32)size);
		else
		{
			if (tools::GetSocketError() == EAGAIN)
				return true;
			return false;
		}
	}
	return true;
}

IOEngineEpoll::IOEngineEpoll(s32 size):_size(size),_epFd(-1), _terminate(false), _mainQueue(QUEUE_SIZE), _threadQueue(QUEUE_SIZE)
{
	_events = (epoll_event *)malloc(sizeof(epoll_event) * _size);
}

bool IOEngineEpoll::Init(IKernel *kernel)
{
	signal(SIGPIPE, SIG_IGN);
	_kernel = kernel;
	_epFd = epoll_create(_size);
	if (_epFd == -1)
	{
		TRACE_LOG("Epoll Init error, error code:%d", tools::GetSocketError());
		return false;
	}
	_workThread = std::thread(&IOEngineEpoll::Run, this);
	_terminate = false;

	TRACE_LOG("Epoll Engine Init Success");
	return true;
}

bool IOEngineEpoll::Stop(IKernel *kernel)
{
	return true;
}

void IOEngineEpoll::Process(IKernel *kernel, s64 tick)
{
	DrivierEvent evt; ;
	while (_mainQueue.Pop(evt))
	{
		if (evt.type == BIND)
			evt.dirver->OnEstablish();
		if (evt.type == UNBIND)
		{
			auto iter = _readyDelDriver.find(evt.dirver->GetNetSocket());
			if (iter != _readyDelDriver.end())
				_readyDelDriver.erase(iter);
			evt.dirver->OnClose();
		}
	}

	for (auto iter = _readyDelDriver.begin(); iter != _readyDelDriver.end(); )
	{
		if (tick - iter->second.tick >= IIOEngine::DELAY_SEND * 1000)
		{
			DrivierEvent unBindEvt;
			unBindEvt.type = UNBIND;
			unBindEvt.dirver = iter->second.dirver;
			unBindEvt.socket = iter->second.dirver->GetNetSocket();
			_threadQueue.TryPush(unBindEvt);
			iter = _readyDelDriver.erase(iter);
			continue;
		}
		iter++;
	}
}

bool IOEngineEpoll::AddIODriver(IIODriver *ioDriver)
{
	IKernel *kernel = _kernel;
	TRACE_LOG("Engine:%d, Add IO Driver Event1", GetId());
	EpollDriver *driver = dynamic_cast<EpollDriver*>(ioDriver);
	ASSERT(driver != nullptr, "error");
	if (nullptr != driver)
	{
		TRACE_LOG("Add IO Driver Event");
		DrivierEvent evt;
		evt.type = BIND;
		evt.dirver = driver;
		evt.socket = driver->GetNetSocket();
		_threadQueue.TryPush(evt);

		return true;
	}

	return false;
}

bool IOEngineEpoll::RemoveIODriver(IIODriver *ioDriver)
{
	IKernel *kernel = _kernel;
	TRACE_LOG("Engine:%d, Remove IO Driver", GetId());
	EpollDriver *driver = dynamic_cast<EpollDriver*>(ioDriver);
	ASSERT(driver != nullptr, "error");
	if (nullptr != driver)
	{
		if (driver->GetSendBuff()->DataSize() == 0)
		{
			DrivierEvent evt;
			evt.type = SHUT_DWON;
			evt.dirver = driver;
			evt.socket = driver->GetNetSocket();
			_threadQueue.TryPush(evt);
		}
		else
		{
			driver->ActiveClose();
			ReadyDelDrivier node;
			node.dirver = driver;
			node.tick = tools::GetTimeMillisecond();
			_readyDelDriver.insert(std::make_pair(driver->GetNetSocket(), node));
		}

		return true;
	}

	return false;
}

IIODriver * IOEngineEpoll::CreateDriver(TcpConnection *connection)
{
	auto iter = _driversMain.find(connection->GetSessionId());
	if (iter == _driversMain.end())
	{
		EpollDriver *driver = NEW EpollDriver(connection);
		AddIODriver(driver);
		IKernel *kernel = _kernel;
		TRACE_LOG("Insert drivermain, sessionId:%d", connection->GetSessionId());
		_driversMain.emplace(connection->GetSessionId(), driver);
		++_driverCount;

		return driver;
	}

	return nullptr;
}

void IOEngineEpoll::RemoveConnection(TcpConnection *connection)
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

void IOEngineEpoll::Run()
{
	IKernel *kernel = _kernel;
	while (true)
	{
		FlushData();
		DrivierEvent evt;
		while (_threadQueue.Pop(evt))
		{
			THREAD_LOG("EpollEvent", "Rcv evt: socket:%d, optType:%d", evt.socket, evt.type);
			if (evt.type == BIND)
				BindEpoll(_kernel, &evt);
			if (evt.type == UNBIND)
				UnBindEpoll(_kernel, &evt);
			if (evt.type == SHUT_DWON)
			{
				shutdown(evt.socket, SHUT_WR);
			}
		}
		s32 count = epoll_wait(_epFd, _events, _size, TIME_OUT);
		if (count == -1)
		{
			if (tools::GetSocketError() == EINTR)
			{
				continue;
			}
			THREAD_LOG("Epoll", "Epoll wait error, error code:%d", tools::GetSocketError());
			break;
		}

		for (s32 i = 0; i < count; i++)
		{
			if (!((EpollDriver*)_events[i].data.ptr)->OnEvent(_kernel, _events[i].events))
			{
				DrivierEvent tmpEvt;
				tmpEvt.type = UNBIND;
				tmpEvt.dirver = (EpollDriver*)_events[i].data.ptr;
				tmpEvt.socket = tmpEvt.dirver->GetNetSocket();
				UnBindEpoll(_kernel, &tmpEvt);
			}
		}

		if (_terminate)
			break;
	}
}

void IOEngineEpoll::BindEpoll(IKernel *kernel, DrivierEvent *evt)
{
	THREAD_LOG("BindEpoll", "BindEpoll evt->socket:%d", evt->socket);
	auto iter = _ctlAddDrivers.find(evt->dirver->GetNetSocket());
	if (iter != _ctlAddDrivers.end())
	{
		THREAD_LOG("IOEngine", "Bind epoll error, evt->socket:%d", evt->socket);
		return;
	}
	else
	{
		epoll_event epollEv;
		epollEv.data.ptr = evt->dirver;
		epollEv.events = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP | EPOLLERR | EPOLLHUP;
		if (epoll_ctl(_epFd, EPOLL_CTL_ADD, evt->socket, &epollEv) != 0)
		{
			THREAD_LOG("IOEngine", "epoll ctl add error, evt->socket:%d, error:%d", evt->socket, tools::GetSocketError());
			evt->type = UNBIND;
			_mainQueue.TryPush(*evt);
			return;
		}
		THREAD_LOG("IOEngine", "epoll ctl add ok, evt->socket:%d", evt->socket);
		_ctlAddDrivers.emplace(evt->dirver->GetNetSocket(), evt->dirver);
	}
	_mainQueue.TryPush(*evt);

	if (!evt->dirver->OnRead())
	{
		evt->type = UNBIND;
		UnBindEpoll(kernel, evt);
	}
}

void IOEngineEpoll::UnBindEpoll(IKernel *kernel, DrivierEvent *evt)
{
	ASSERT(evt->dirver != nullptr, "error");
	auto iter = _ctlAddDrivers.find(evt->socket);
	if (iter == _ctlAddDrivers.end())
	{
		THREAD_LOG("IOEngine", "epoll ctl had del, evt->socket:%d", evt->socket);
		return;
	}
	else
		_ctlAddDrivers.erase(iter);
	//shutdown(evt->socket, SHUT_WR);
	//evt->dirver->RecvFin();
	if (epoll_ctl(_epFd, EPOLL_CTL_DEL, evt->socket, nullptr) != 0)
	{
		THREAD_LOG("IOEngine", "epoll ctl del error, evt->socket:%d, error:%d", evt->socket, tools::GetSocketError());
	}
	else
	{
		THREAD_LOG("IOEngine", "epoll ctl del success, evt->socket:%d", evt->socket)
	}

	_mainQueue.TryPush(*evt);
}

void IOEngineEpoll::FlushData()
{
	for (auto iter : _ctlAddDrivers)
		iter.second->OnSend();
}
#endif

