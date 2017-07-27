#ifndef __TimerMgr_h__
#define __TimerMgr_h__
#include "ITimerMgr.h"
#include "TString.h"
#include "TimerWheel.h"

class TimerMgr : public ITimerMgr
{
public:
    CREATE_INSTANCE(TimerMgr);

	virtual bool Ready();
	virtual bool Initialize() { return true; };
	virtual bool Destroy() { return true; };

    virtual void Process(s32 tick);
    virtual void StartTimer(core::ITimer *timer, s32 delay, s32 count, s32 interval, const char *trace);
    virtual void KillTimer(core::ITimer *timer);
protected:
private:
	TimerWheel  _timerWheel;
	core::IKernel   * _kernel;
};
#endif
