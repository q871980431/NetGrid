/*
 * File:	NetService.cpp
 * Author:	xuping
 * 
 * Created On 2019/4/23 10:59:11
 */

#include "TcpConnection.h"
#include "NetService.h"
#include "NetIOEngine.h"
#include "../Exception/ExceptionMgr.h"

TcpConnection::TcpConnection(s32 sessionId, bool passive, NetSocket netSocket, ITcpSession *session, NetService *service)
{
	_sessionId = sessionId;
	_passive = passive;
	_netSocket = netSocket;
	_ioEngine = nullptr;
	_ioDriver = nullptr;
	_tcpSession = session;
	_establish = false;
	_service = service;
	_sendBuffSize = ITcpConnection::INIT_SEND_BUFF_SIZE;
	_recvBuffSize = ITcpConnection::INIT_RECV_BUFF_SIZE;
}

void TcpConnection::Send(const char *buff, s32 len)
{
	if (len > 0)
	{
		CircluarBuffer *sendBuff = _ioDriver->GetSendBuff();
		s32 sendLen = sendBuff->Write(buff, len);
		if (sendLen != len)
		{
			IKernel *kernel = _service->GetKernel();
			ERROR_LOG("Session:%d, Send buff full, Froce Close", _sessionId);
		}
	}
}

const char * TcpConnection::GetRemoteIP()
{
	return tools::GetSocketIp(_romoteAddr);
}

s32 TcpConnection::GetRemoteIpAddr()
{
	return 0;
}

const char * TcpConnection::GetCloseReason()
{
	return _closeReason.c_str();
}

bool TcpConnection::SettingBuffSize(s32 recvSize, s32 sendSize)
{
	ASSERT(recvSize != 0 && sendSize != 0, "size error");
	if (recvSize == 0 || sendSize == 0)
		return false;

	if (_ioDriver) {
		if (!_ioDriver->SettingBuffSize(recvSize, sendSize))
			return false;
	}
	_sendBuffSize = recvSize;
	_recvBuffSize = recvSize;
	return true;
}

void TcpConnection::Init()
{
	_tcpSession->SetConnection(this);
	tools::GetRomoteSocketInfo(_netSocket, _romoteAddr);
}


void TcpConnection::Close(const char *reason)
{
	ASSERT(_tcpSession, "error");
	if (_tcpSession)
	{
		if (reason != nullptr)
			_closeReason = reason;

		_tcpSession->SetConnection(nullptr);
		_tcpSession->OnTerminate();
		_tcpSession->OnRelease();
		_tcpSession = nullptr;
		_ioEngine->RemoveIODriver(_ioDriver);
	}
}

void TcpConnection::OnEstablish()
{
	ASSERT(!_establish, "error");
	if (!_establish)
	{
		_tcpSession->OnEstablish();
		_establish = true;
	}
}

void TcpConnection::OnTerminate(bool recvFin, s32 errorCode)
{
	//tcpsession == null 标志着已经主动close了
	if (_tcpSession)
	{
		char buff[256];
		SafeSprintf(buff, sizeof(buff), "remote close, fin:%d, error code:%d", recvFin, errorCode);
		_closeReason = buff;

		_tcpSession->SetConnection(nullptr);
		_tcpSession->OnTerminate();
		_tcpSession->OnRelease();
		_tcpSession = nullptr;
	}

	_service->RemoveConnection(_sessionId, recvFin);
}

void TcpConnection::OnRecv(IKernel *kernel)
{
	CircluarBuffer *rcvBuff = nullptr;
	s32 packetLen = 0;
	s32 len = 0;
	while (true)
	{
		if (_tcpSession == nullptr)
			return;

		rcvBuff = _ioDriver->GetRecvBuff();			//可能修改该rcvBuffPtr
		packetLen = _tcpSession->OnParsePacket(rcvBuff);
		if (packetLen == 0)
			break;
		if (packetLen == ITcpSession::INVALID_PACKET_LEN)
		{
			ERROR_LOG("parse packet error, socket:%d", _netSocket);
			Close("parse packet error");
			return;
		}
		if (packetLen > rcvBuff->DataSize())
			break;

		char *buff = rcvBuff->GetCanReadBuff(len);
		if (len < packetLen)
		{
			buff = (char *)alloca(packetLen);
			len = rcvBuff->Read(buff, packetLen);
			ASSERT(len == packetLen, "error");
			TRY_BEGIN
			_tcpSession->OnRecv(buff, packetLen);
			TRY_END
		}
		else
		{
			TRY_BEGIN
			_tcpSession->OnRecv(buff, packetLen);
			TRY_END
			rcvBuff->Read(packetLen);
		}
	}
	return;
}
