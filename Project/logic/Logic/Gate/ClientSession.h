#ifndef __GateSession_h__
#define __GateSession_h__
#include "IKernel.h"
#pragma  pack (push, 4)             //按4字节对齐
struct ClientMsgHead 
{
	s32 messageId;
	s32 len;
};
#pragma  pack (pop)                 //恢复缺省对齐

class Client;
class ClientSession : public core::ITcpSession
{
public:
	ClientSession(s32 indexId): _indexId(indexId) {};
	virtual void  SetConnection(core::ITcpConnection *connection) { _connection = connection; };
	virtual void  OnEstablish();
	virtual void  OnTerminate() {};
	virtual void  OnError(s32 moduleErr, s32 sysErr);
	virtual void  OnRecv(const char *buff, s32 len);
	virtual s32	  OnParsePacket(CircluarBuffer *recvBuff);
	virtual void  OnRelease();

public:
	inline void SendData(const void *buff, s32 len) {if (_connection)_connection->Send((char*)buff, len);};
	void SendMsg(s32 messageId, const void *buff, s32 len)
	{
		ClientMsgHead head;
		head.messageId = messageId;
		head.len = len + sizeof(ClientMsgHead);
		if (_connection)
		{
			_connection->Send((const char *)&head, sizeof(ClientMsgHead));
			_connection->Send((const char *)buff, len);
		}
	}

	void Close(const char *reason);

public:
	inline void BindClient(Client *client) { _client = client; };
	inline Client * GetClient() { return _client; };

protected:
private:
	s32		_indexId;
	Client	*_client;
	core::ITcpConnection *_connection;

	static s32	s_sendBuffSize;
	static s32  s_recvBuffSIze;
};

#endif
