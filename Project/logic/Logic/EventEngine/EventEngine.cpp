/*
 * File:	EventEngine.cpp
 * Author:	xuping
 * 
 * Created On 2017/9/24 15:43:19
 */

#include "EventEngine.h"
EventEngine * EventEngine::s_self = nullptr;
IKernel * EventEngine::s_kernel = nullptr;
EventMap		EventEngine::s_eventMap;
EventLockMap	EventEngine::s_eventLockMap;
JudgeListMap	EventEngine::s_judgeMap;

bool EventEngine::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool EventEngine::Launched(IKernel *kernel)
{

    return true;
}

bool EventEngine::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

void EventEngine::AddEventListener(s32 eventid, EVENT_CALL_BACK callback, const char *debug /* = nullptr */)
{
	EventListener listener;
	listener.eventId = eventid;
	listener.f = callback;
	listener.debug = debug;
	auto iter = s_eventMap.find(eventid);
	if (iter == s_eventMap.end())
	{
		auto ret = s_eventMap.insert(std::make_pair(eventid, EventList()));
		ASSERT(ret.second, "error");
		ret.first->second.push_back(listener);
	}else
		iter->second.push_back(listener);
}

void EventEngine::ActiveEvent(s32 eventid, const void *context, s32 size)
{
	auto iter = s_eventMap.find(eventid);
	if (iter != s_eventMap.end())
	{
		for (const auto listener : iter->second)
			listener.f(s_kernel, context, size);
	}
}

bool EventEngine::CreateEventLock(s64 eventid, UnLockCallFunction &callback)
{
	auto iter = s_eventLockMap.find(eventid);
	ASSERT(callback != nullptr, "error");
	ASSERT(false, "error")
	if (callback == nullptr)
		return false;
	if (iter != s_eventLockMap.end())
	{
		EventLock lock;
		lock.eventId = eventid;
		lock.lockNum = 0;
		lock.f = callback;
		s_eventLockMap.insert(std::make_pair(eventid, lock));
		return true;
	}

	return false;
}

s32 EventEngine::GetEventLockId(s64 eventid)
{
	auto iter = s_eventLockMap.find(eventid);
	if (iter == s_eventLockMap.end())
	{
		ASSERT(false, "error");
		return UNVALID_LOCK_ID;
	}
	if (iter->second.lockNum >= core::EVENT_LOCK_NUM_MAX)
	{
		ASSERT(false, "error");
		return false;
	}
	s32 rt = iter->second.lockNum++;
	iter->second.bits.set(rt);
	return rt;
}

bool EventEngine::RemoveEventLock(s64 eventid, s32 lockid)
{
	auto iter = s_eventLockMap.find(eventid);
	if (iter == s_eventLockMap.end())
	{
		ASSERT(false, "error");
		return false;
	}
	if (lockid >= iter->second.lockNum)
	{
		ASSERT(false, "error");
		return false;
	}
	if (!iter->second.bits.test(lockid))
	{
		ASSERT(false, "lockid has unlock");
		return false;
	}
	iter->second.bits.reset(lockid);
	if (iter->second.bits.none())
		iter->second.f(s_kernel, eventid);

	return true;
}

void EventEngine::AddJudgeListener(s32 eventid, JUDGE_CALL_BACK callback, const char *debug /* = nullptr */)
{
	JudgeListener listener;
	listener.eventId = eventid;
	listener.f = callback;
	listener.debug = debug;
	auto iter = s_judgeMap.find(eventid);
	if (iter == s_judgeMap.end())
	{
		auto ret = s_judgeMap.insert(std::make_pair(eventid, JudgeList()));
		iter = ret.first;
	}
	iter->second.push_back(listener);
}

bool EventEngine::ActiveJudge(s32 eventid, const void *context, s32 size, void *userdata)
{
	auto iter = s_judgeMap.find(eventid);
	if (iter != s_judgeMap.end())
	{
		for (auto listener : iter->second)
		{
			if (!listener.f(s_kernel, context, size, userdata))
				return false;
		}
	}

	return true;
}
