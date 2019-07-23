/*
 * File:	IEventEngine.h
 * Author:	xuping
 * 
 * Created On 2017/9/24 15:43:19
 */

#ifndef  __IEventEngine_h__
#define __IEventEngine_h__
#include "IModule.h"
#include <functional>
class IEventEngine : public IModule
{
public:
	typedef void(*EVENT_CALL_BACK)(IKernel *kernel, const void *context, s32 size);
	typedef bool(*JUDGE_CALL_BACK)(IKernel *kernel, const void *context, s32 size, void *userdata);
	typedef std::function<void(IKernel *kernel, s64 eventId)> UnLockCallFunction;
	static const s32 UNVALID_LOCK_ID = -1;

    virtual ~IEventEngine(){};
    
	virtual void AddEventListener(s32 eventid, EVENT_CALL_BACK callback, const char *debug = nullptr) = 0;
	virtual void ActiveEvent(s32 eventid, const void *context, s32 size) = 0;
	virtual bool CreateEventLock(s64 eventid, UnLockCallFunction &callback) = 0;
	virtual s32  GetEventLockId(s64 eventid) = 0;
	virtual bool RemoveEventLock(s64 eventid, s32 lockid) = 0;
	virtual void AddJudgeListener(s32 eventid, JUDGE_CALL_BACK callback, const char *debug = nullptr) = 0;
	virtual bool ActiveJudge(s32 eventid, const void *context, s32 size, void *userdata) = 0;
	//virtual void AddTriggerListener(s32 eventid, EVENT_CALL_BACK callback, const char *debug = nullptr) = 0;
};

#define REG_EVENT_LISTENER(eventEngine, eventId, callBack) eventEngine->AddEventListener(eventId, callBack, __FILE__)

#endif