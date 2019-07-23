#ifndef __HarborSession_h__
#define __HarborSession_h__
#include "IKernel.h"
#include "Harbor.h"
#include "TString.h"

typedef tlib::TString<64> IPString;
class DelayOpenTimer;
class HarborSession : public core::ITcpSession
{
public:
	HarborSession(Harbor *harbor, IKernel *kernel) :_harbor(harbor), _kernel(kernel), _connection(nullptr), _nodeType(0), _nodeId(0) , _port(0), _activeClose(false) {};
	virtual ~HarborSession() {};
	virtual void  SetConnection(ITcpConnection *connection) { 
		if (_connection && connection == nullptr)
			TRACE_LOG("connection close reason:%s", _connection->GetCloseReason());
		_connection = connection; 
	};
	virtual void  OnEstablish() { TRACE_LOG("session enter establish, type:%d, nodeId:%d", _nodeType, _nodeId); _harbor->OnOpen(this); };
	virtual void  OnTerminate() { TRACE_LOG("session enter terminate, type:%d, nodeId:%d", _nodeType, _nodeId); _harbor->OnClose(this); };
	virtual void  OnError(s32 moduleErr, s32 sysErr) { TRACE_LOG("session enter error, type:%d, nodeId:%d", _nodeType, _nodeId); };
	virtual s32	  OnParsePacket(CircluarBuffer *recvBuff);
	virtual void  OnRecv(const char *buff, s32 len);
	virtual void  OnRelease() { DEL this; }

public:
	virtual void Close(const char *reason) {
		_activeClose = true;
		if (_connection)
			_connection->Close(reason);
	};

public:
	inline void SetNodeInfo(s8 type, s8 nodeId) { _nodeType = type; _nodeId = nodeId; };
	inline void SendBuff(const char *buff, s32 len) { _connection->Send(buff, len); };
	inline void SendMsg(s32 messageId, const char *buff, s32 len) {
		MessageHead head;
		head.messageId = messageId;
		head.len = len + sizeof(MessageHead);
		_connection->Send((const char *)&head, sizeof(MessageHead));
		_connection->Send(buff, len);
	};
	inline s8 GetNodeType() { return _nodeType; };
	inline s8 GetNodeID() { return _nodeId; };
	inline const char *GetRemoteIP() { return _connection->GetRemoteIP(); };
	inline bool SettingBuffSize(s32 recvSize, s32 sendSize) { return _connection->SettingBuffSize(recvSize, sendSize); };
	inline void SetDelayTimer(DelayOpenTimer *delyOpenTimer) { _delayOpenTimer = delyOpenTimer; };
	inline DelayOpenTimer * GetDelayTimer() { return _delayOpenTimer; };
	inline void SetIpInfo(const char *ip, s32 port) { _ip = ip; _port = port; };
	inline const char * GetSessionIp() { return _ip.GetString(); };
	inline s32			GetSessionPort() { return _port; };

private:
	void  OnRecv(s32 messageId, const char *buff, s32 len) { _harbor->OnRecv(this, messageId, buff, len); };

protected:
	Harbor  * _harbor;
	IKernel     *_kernel;
	ITcpConnection  *_connection;
	s8				_nodeType;
	s8				_nodeId;
	DelayOpenTimer	*_delayOpenTimer;
	IPString		_ip;
	s32				_port;
	bool			_activeClose;
};

class ReConnectTimer;
class HarborClientSession : public HarborSession
{
public:
	HarborClientSession(IKernel *kernel, Harbor *harbor, const char *host, s32 port, s64 channel) :HarborSession(harbor, kernel) {
		_host.ip = host;
		_host.port = port;
		_connectCount = 0;
		_timer = nullptr;
		_channel = channel;
		_connecting = false;
	}
	virtual ~HarborClientSession();
	virtual void  OnEstablish();
	virtual void  OnError(s32 moduleErr, s32 sysErr);
	virtual void  OnRelease();

	virtual void Close(const char *reason) {
		_activeClose = true;
		TRACE_LOG("Client active close, connection:%p, connecting:%d", _connection, _connecting);
		if (_connection)
			_connection->Close(reason);
		else if (_connecting == false)
		{
			OnRelease();
		}
	};

public:
	inline IKernel * GetKernel() { return _kernel; };
	inline HostInfo * GetHostInfo() { return &_host; };
	inline void OnTimerTerminate() { _timer = nullptr; };
	inline s64	GetChannel() { return _channel; };
	inline void SetConnecting() { _connecting = true; };

private:
	HostInfo    _host;
	s8          _connectCount;
	ReConnectTimer *_timer;
	s64			_channel;
	bool		_connecting;
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
		TRACE_LOG("reconnect hostip:%s, port:%d", hostInfo->ip.GetString(), hostInfo->port);
		_session->SetConnecting();
		_session->GetKernel()->CreateNetSession(hostInfo->ip.GetString(), hostInfo->port, _session);
	};
	virtual void OnTerminate(IKernel *kernel, s64 tick, bool isKill) { _session->OnTimerTerminate(); DEL this; };
protected:
	HarborClientSession *_session;
};

class DelayOpenTimer : public ITimer
{
public:
	DelayOpenTimer(HarborSession *session, Harbor *harbor) :_harbor(harbor),_session(session) {};
	virtual ~DelayOpenTimer() {};

	virtual void OnStart(IKernel *kernel, s64 tick) {};
	virtual void OnTime(IKernel *kernel, s64 tick) {
		_harbor->OnTimerSessionDelayOpen(_session);
	};
	virtual void OnTerminate(IKernel *kernel, s64 tick, bool isKill) { DEL this; };
protected:
private:
	Harbor		  *_harbor;
	HarborSession *_session;
};
#endif
