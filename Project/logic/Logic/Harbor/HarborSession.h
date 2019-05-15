#ifndef __HarborSession_h__
#define __HarborSession_h__
#include "IKernel.h"
#include "Harbor.h"
class HarborSession : public core::ITcpSession
{
public:
	HarborSession(Harbor *harbor, IKernel *kernel) :_harbor(harbor), _type(0), _nodeId(0), _connection(nullptr), _kernel(kernel) {};
	virtual void  SetConnection(ITcpConnection *connection) { _connection = connection; };
	virtual void  OnEstablish() { _harbor->OnOpen(this); };
	virtual void  OnTerminate() { _harbor->OnClose(this); DEL this; };
	virtual void  OnError(s32 moduleErr, s32 sysErr) {};
	virtual s32	  OnParsePacket(CircluarBuffer *recvBuff);
	virtual void  OnRecv(const char *buff, s32 len);

public:
	inline void SetNodeInfo(s32 type, s32 nodeId) { _type = type; _nodeId = nodeId; };
	inline void SendBuff(const char *buff, s32 len) { _connection->Send(buff, len); };
	inline void SendMsg(s32 messageId, const char *buff, s32 len) {
		MessageHead head;
		head.messageId = messageId;
		head.len = len + sizeof(MessageHead);
		_connection->Send((const char *)&head, sizeof(MessageHead));
		_connection->Send(buff, len);
	};
	inline s32 GetType() { return _type; };
	inline s32 GetID() { return _nodeId; };
	inline const char *GetRemoteIP() { return _connection->GetRemoteIP(); };
	inline void SettingBuffSize(s32 recvSize, s32 sendSize) { _connection->SettingBuffSize(recvSize, sendSize); };
private:
	void  OnRecv(s32 messageId, const char *buff, s32 len) { _harbor->OnRecv(this, messageId, buff, len); };
protected:
	Harbor  * _harbor;
	IKernel     *_kernel;
	ITcpConnection  *_connection;
	s32       _type;
	s32       _nodeId;
};

class ReConnectTimer;
class HarborClientSession : public HarborSession
{
public:
	HarborClientSession(IKernel *kernel, Harbor *harbor, const char *host, s16 port) :HarborSession(harbor, kernel) {
		_host.ip = host;
		_host.port = port;
		_connectCount = 0;
		_timer = nullptr;
	}
	virtual ~HarborClientSession();

	virtual void  OnTerminate();
	virtual void  OnError(s32 moduleErr, s32 sysErr);
	inline IKernel * GetKernel() { return _kernel; };
	inline HostInfo * GetHostInfo() { return &_host; };
	inline void OnTimerTerminate() { _timer = nullptr; };
	void  StartTimer();
private:
	HostInfo    _host;
	s8          _connectCount;
	ReConnectTimer *_timer;
};

class ReConnectTimer : public ITimer
{
public:
	ReConnectTimer(HarborClientSession *session) :_session(session) {
	};
	virtual ~ReConnectTimer() {};

	virtual void OnStart(IKernel *kernel, s64 tick) {};
	virtual void OnTime(IKernel *kernel, s64 tick) {
		HostInfo *hostInfo = _session->GetHostInfo();
		_session->GetKernel()->CreateNetSession(hostInfo->ip.GetString(), hostInfo->port, _session);
	};
	virtual void OnTerminate(IKernel *kernel, s64 tick) { _session->OnTimerTerminate(); DEL this; };
protected:
	HarborClientSession *_session;
};
#endif
