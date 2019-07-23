/*
 * File:	IHarbor.h
 * Author:	XuPing
 * 
 * Created On 2017/9/3 22:51:33
 */

#ifndef  __IHarbor_h__
#define __IHarbor_h__
#include "IModule.h"
class INodeListener
{
public:
    virtual ~INodeListener(){};

    virtual void OnOpen(s8 type, s8 nodeId, const char *ip, s32 port) = 0;
    virtual void OnClose(s8 type, s8 nodeId) = 0;
private:
};

typedef void(*CALL_BACK)(s8 type, s8 nodeId, const char *buff, s32 len);

struct HarborAddr 
{
	s32 areaId;	//subnetId
	s8	nodeType;
	s8	nodeId;
};

const static s32 HARBOR_AREA_SELF = -1;
const static s32 HARBOR_RECONNECT_TIME = 5;	//Second

class IHarbor : public IModule
{
public:
    virtual ~IHarbor(){};
    virtual void AddListener(INodeListener *listener) = 0;
    virtual void RegisterMessage(s32 messageId, const CALL_BACK fun) = 0;
    virtual void Sendbuff(s8 type, s8 nodeId, const void *buff, s32 len) = 0;
    virtual void SendMessage(s8 type, s8 nodeId, s32 messageId, const void *buff, s32 len) = 0;
	virtual const HarborAddr * GetHarborAddr() = 0;

public:
	virtual bool ConnectHarbor(s8 type, s8 nodeId, const char *ip, s32 port, bool force = false) = 0;
	virtual bool DisconnectHarbor(s8 type, s8 nodeId, const char *reason) = 0;
};

#endif