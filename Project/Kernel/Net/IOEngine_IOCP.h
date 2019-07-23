/*
 * File:	NetService.h
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#ifndef __IOEngine_IOCP_h__
#define __IOEngine_IOCP_h__
//#define LINUX
#ifdef WIN32
#include "NetIOEngine.h"
#include "CircularQueue.h"
#include <unordered_map>
#include <mutex>

class IocpDriver : public IIODriver
{
	enum OPT_TYPE
	{
		SEND = 1,
		RECV = 2,
	};
	struct PerIoOperationData
	{
		OVERLAPPED overlapped;
		WSABUF dataBuff;
		OPT_TYPE opType;
	};
public:
	IocpDriver(TcpConnection *connection, HANDLE complatPort);
	virtual bool SettingBuffSize(s32 recvSize, s32 sendSize);

	void OnError(IKernel *kernel, OVERLAPPED *perIoOperation);
	void OnEvent(IKernel *kernel, OVERLAPPED *perIoOperation, s32 transferSize);

	bool PostRecv(IKernel *kernel);
	void BindCompletionPort();
	bool CheckClose();
	bool PostSend();
	inline bool Sending() { return _sending; };

protected:
	bool OnSend(s32 transferSize);
	bool OnRead(IKernel *kernel, s32 transferSize);
	void EnterClose();

private:
	HANDLE	_complationPort;

	PerIoOperationData	_sendPerData;
	PerIoOperationData	_recvPerData;
	std::mutex			_sendMutex;
	bool				_sending;
	bool				_recving;
	CircluarBuffer		*_nextRecvBuff;
};

class IOEngineIocp : public IIOEngine
{
	const static s32 QUEUE_SIZE = 2048;
	const static s32 TIME_OUT = 10;
	struct DrivierEvent 
	{
		IocpDriver *dirver;
		NetSocket socket;
		bool bind;
	};

	struct ReadyDelDrivier
	{
		IocpDriver *dirver;
		s64 tick;
	};

public:
	IOEngineIocp(s32 threadSize);

	virtual bool Init(IKernel *kernel);
	virtual bool Stop(IKernel *kernel);
	virtual void Process(IKernel *kernel, s64 tick);

	virtual bool AddIODriver(IIODriver *ioDriver);
	virtual bool RemoveIODriver(IIODriver *ioDriver);
	virtual IIODriver * CreateDriver(TcpConnection *connection);
	virtual void RemoveConnection(TcpConnection *connection);

public:
	void EnterDel(IocpDriver *driver);

private:
	void Run();

private:
	s32 _threadSize;
	HANDLE _completionPort;
	std::vector<std::thread> _workThread;
	bool _terminate;
	IKernel *_kernel;

	CircluarQueue<DrivierEvent>	_threadQueue;
	CircluarQueue<DrivierEvent> _mainQueue;
	HANDLE	*_events;
	std::unordered_map<s32, IocpDriver *> _driversMain;
	std::vector<IocpDriver *> _dels;
	std::mutex			   _delMutex;
	std::unordered_map<s32, IocpDriver *> _hasDel;
	std::unordered_map<s32, ReadyDelDrivier> _readyDelDriver;
};
#endif
#endif