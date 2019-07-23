/*
 * File:	NetService.h
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#ifndef __IOEngine_Epoll_h__
#define __IOEngine_Epoll_h__
//#define LINUX
#ifdef LINUX
#include "NetIOEngine.h"
#include "CircularQueue.h"
#include <sys/epoll.h>
#include <unordered_map>

class EpollDriver : public IIODriver
{
public:
	EpollDriver(TcpConnection *connection) : IIODriver(connection) {};

	virtual bool SettingBuffSize(s32 recvSize, s32 sendSize);
	bool OnEvent(IKernel *kernel, u32 events);
	bool OnSend();
	bool OnRead();
protected:
private:
};

class IOEngineEpoll : public IIOEngine
{
	const static s32 QUEUE_SIZE = 2048;
	const static s32 TIME_OUT = 10;
	enum EventType
	{
		BIND = 1,
		UNBIND = 2,
		SHUT_DWON = 3,
	};

	struct DrivierEvent 
	{
		EpollDriver *dirver;
		NetSocket socket;
		EventType type;
	};

	struct ReadyDelDrivier 
	{
		EpollDriver *dirver;
		s64 tick;
	};

public:
	IOEngineEpoll(s32 size);

	virtual bool Init(IKernel *kernel);
	virtual bool Stop(IKernel *kernel);
	virtual void Process(IKernel *kernel, s64 tick);

	virtual bool AddIODriver(IIODriver *ioDriver);
	virtual bool RemoveIODriver(IIODriver *ioDriver);
	virtual IIODriver * CreateDriver(TcpConnection *connection);
	virtual void RemoveConnection(TcpConnection *connection);

private:
	void Run();
	void BindEpoll(IKernel *kernel, DrivierEvent *evt);
	void UnBindEpoll(IKernel *kernel, DrivierEvent *evt);
	void FlushData();
private:
	s32 _size;
	s32 _epFd;
	std::thread _workThread;
	bool _terminate;
	IKernel *_kernel;

	CircluarQueue<DrivierEvent> _mainQueue;
	CircluarQueue<DrivierEvent>	_threadQueue;
	epoll_event	*_events;
	std::unordered_map<s32, EpollDriver *> _driversMain;
	std::unordered_map<s32, EpollDriver *> _ctlAddDrivers;
	std::unordered_map<s32, ReadyDelDrivier> _readyDelDriver;
};
#endif
#endif