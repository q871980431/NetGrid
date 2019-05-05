/*
 * File:	NetService.h
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#ifndef __NetIOEngine_h__
#define __NetIOEngine_h__
#include "INetService.h"
#include "TcpConnection.h"
#include "CircularBuffer.h"
#include <thread>
#include <vector>

class IIODriver;
class IIOEngine
{
public:
	IIOEngine() :_driverCount(0){};
	virtual ~IIOEngine() {};

	virtual bool Init(IKernel *kernel) = 0;
	virtual bool Stop(IKernel *kernel) = 0;
	virtual void Process(IKernel *kernel, s64 tick) = 0;
	virtual bool AddIODriver(IIODriver *ioDriver) = 0;
	virtual bool RemoveIODriver(IIODriver *ioDriver) = 0;

	virtual IIODriver * CreateDriver(TcpConnection *connection) = 0;
	virtual void RemoveIODriver(TcpConnection *connection) = 0;
public:
	s32 GetDriverCount() { return _driverCount; }
	inline void SetId(s32 id) { _id = id; };
	inline s32  GetId() { return _id; };
protected:
	s32	_driverCount;
	s32 _id;
};

class IIODriver
{
public:
	IIODriver(TcpConnection *connection);
	virtual ~IIODriver();

	NetSocket GetNetSocket() { return _connetion->GetNetSocket(); };
	void OnEstablish() { _connetion->OnEstablish(); };
	void OnClose() { _close = true; _connetion->OnTerminate(_recvFin); };
	void SettingBuffSize(s32 recvSize, s32 sendSize);
	inline bool HasClose() { return _close; }
	inline void RecvFin() { _recvFin = true; }
	inline CircluarBuffer * GetRecvBuff() { return _recvBuff; };
	inline CircluarBuffer * GetSendBuff() { return _sendBuff; };
protected:
	TcpConnection *_connetion;

	CircluarBuffer *_sendBuff;
	CircluarBuffer *_recvBuff;
	bool _close;
	bool _recvFin;
};

class NetIOEngine
{
public:
	NetIOEngine(s32 engineNum);
	bool Init(IKernel *kernel);
	bool AddConnection(TcpConnection *connection);
	bool RemoveConnection(TcpConnection *connection);
	void Process(IKernel *kernel, s64 tick);

private:
	IIOEngine * GetExecIOEngine();
private:
	s32 _engineNum;
	std::vector<IIOEngine*>	_ioengines;
};
#endif