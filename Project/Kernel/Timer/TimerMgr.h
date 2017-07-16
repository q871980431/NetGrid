#ifndef __TimerMgr_h__
#define __TimerMgr_h__
#include "ITimerMgr.h"
#include "TString.h"

class TimerBase : public core::ITrace
{
public:
    TimerBase(core::ITimer *timer, s32 count, s32 interval, const char *trace) :_trace(trace){
        _timer = timer;
        _count = count;
        _interval = interval;
    };
    void OnTime();
    inline void SetExpires(u32 expires){ _expires = expires; };
protected:
private:
    core::ITimer *_timer;
    u32 _expires;
    s32 _count;
    s32 _interval;
    tlib::TString<TRACE_LEN> _trace;
};

class TimerMgr : public ITimerMgr
{
public:
    CREATE_INSTANCE(TimerMgr);

    virtual bool Ready();
    virtual bool Initialize();
    virtual bool Destroy();

    virtual void Process(s32 tick);
    virtual void StartTimer(core::ITimer *timer, s32 delay, s32 count, s32 interval, const char *trace);
    virtual void KillTimer(core::ITimer *timer);
protected:
private:
};
#endif
