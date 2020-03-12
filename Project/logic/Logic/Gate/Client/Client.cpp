#include "Client.h"
#include "ClientSession.h"
#include "FrameworkDefine.h"

Client::Client()
{
	_session = nullptr;
	_heartBeat = core::CLIENT_HEART_BEAT_TIME;
}

void Client::BindSession(ClientSession *session)
{
	_session = session;
	_session->BindClient(this);
}

void Client::StartHeartBeatTimer()
{
	ASSERT(_heartBeatTimer == nullptr, "timer error");
	_heartBeatTimer = new HeartBeatTimer(this);
	ADD_TIMER(_heartBeatTimer, 0, FOREVER, core::CLIENT_HEART_BEAT_TIME);
}

bool Client::CheckHeartBeat(s64 nowTick)
{
	return !(nowTick > _heartTick + _heartBeat);
}