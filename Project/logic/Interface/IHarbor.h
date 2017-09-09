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

    virtual void OnOpen(s32 type, s32 nodeId, const char *ip, s16 port);
    virtual void OnClose(s32 type, s32 nodeId);
private:
};

typedef void (*CALL_BACK)(const char *buff, s32 len);
class IHarbor : public IModule
{
public:
    virtual ~IHarbor(){};
    virtual void AddListener(INodeListener *listener) = 0;
    virtual void RegisterMessage(s32 messageId, CALL_BACK fun) = 0;
    virtual void Sendbuff(s32 type, s32 nodeId, const char *buff, s32 len) = 0;
    virtual void SendMessage(s32 type, s32 nodeId, s32 messageId, const char *buff, s32 len) = 0;

};
#endif