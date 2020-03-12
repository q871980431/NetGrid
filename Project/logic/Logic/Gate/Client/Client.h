#ifndef __Client_h__
#define __Client_h__
#include "IKernel.h"
#include "TIndexPool.h"
#include "ClientSession.h"

class HeartBeatTimer;
class Client : public tlib::IndexNode
{
public:
	Client();


public:
	void BindSession(ClientSession *session);
	inline void SendData(const void *buff, s32 len) { _session->SendData(buff, len); };
	inline void SendMsg(s32 messageId, const void *buff, s32 len) { _session->SendMsg(messageId, buff, len); };
	inline void Close(const char *reason) { _session->Close(reason); };
	inline void SetAuth() { _auth = true; }

public:
	void StartHeartBeatTimer();
	bool CheckHeartBeat(s64 nowTick);

protected:
private:
	ClientSession	*_session;
	bool			_auth;

	s32				_heartBeat;
	HeartBeatTimer	*_heartBeatTimer;
	s64				_heartTick;
};

class HeartBeatTimer : public core::ITimer
{
public:
	HeartBeatTimer(Client *pClient) :_client(pClient) {};
	virtual ~HeartBeatTimer() {};

	virtual void OnStart(core::IKernel *kernel, s64 tick) {};
	virtual void OnTime(core::IKernel *kernel, s64 tick) { 
		if (!_client->CheckHeartBeat(tick))
		{
			_client->Close("heart tick timeout");
		}
	};
	virtual void OnTerminate(core::IKernel *kernel, s64 tick, bool isKill) {};
protected:
private:
	Client	*_client;
};
#endif
