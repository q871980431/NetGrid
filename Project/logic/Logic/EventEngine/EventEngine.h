/*
 * File:	EventEngine.h
 * Author:	xuping
 * 
 * Created On 2017/9/24 15:43:19
 */

#ifndef __EventEngine_h__
#define __EventEngine_h__
#include "IEventEngine.h"
#include "TString.h"
#include "FrameworkDefine.h"
#include <vector>
#include <unordered_map>
#include <bitset>
struct  EventListener
{
	s32 eventId;
	IEventEngine::EVENT_CALL_BACK f;
	tlib::TString<TRACE_LEN> debug;
};

struct EventLock 
{
	s64 eventId;
	s16 lockNum;
	std::bitset<core::EVENT_LOCK_NUM_MAX> bits;
	IEventEngine::UnLockCallFunction f;
};

struct JudgeListener 
{
	s32 eventId;
	IEventEngine::JUDGE_CALL_BACK f;
	tlib::TString<TRACE_LEN> debug;
};

typedef std::vector<EventListener> EventList;
typedef std::vector<JudgeListener> JudgeList;
typedef std::unordered_map<s32, EventList> EventMap;
typedef std::unordered_map<s64, EventLock> EventLockMap;
typedef std::unordered_map<s32, JudgeList> JudgeListMap;
class EventEngine : public IEventEngine
{
public:
    virtual ~EventEngine(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

	virtual void AddEventListener(s32 eventid, EVENT_CALL_BACK callback, const char *debug = nullptr);
	virtual void ActiveEvent(s32 eventid, const void *context, s32 size);
	virtual bool CreateEventLock(s64 eventid, UnLockCallFunction &callback);
	virtual s32  GetEventLockId(s64 eventid);
	virtual bool RemoveEventLock(s64 eventid, s32 lockid);
	virtual void AddJudgeListener(s32 eventid, JUDGE_CALL_BACK callback, const char *debug = nullptr);
	virtual bool ActiveJudge(s32 eventid, const void *context, s32 size, void *userdata);
protected:
private:
    static EventEngine     * s_self;
    static IKernel  * s_kernel;

	static EventMap				s_eventMap;
	static EventLockMap			s_eventLockMap;
	static JudgeListMap			s_judgeMap;
};
#endif