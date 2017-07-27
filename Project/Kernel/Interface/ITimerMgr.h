#ifndef __ITimerMgr_h__
#define __ITimerMgr_h__
#include "MultiSys.h"
#include "ICore.h"
#include "IKernel.h"

class ITimerMgr    : public core::ICore
{
public:

    virtual void Process(s32 tick) = 0;
    virtual void StartTimer( core::ITimer *timer, s32 delay, s32 count, s32 interval, const char *trace) = 0;
    virtual void KillTimer( core::ITimer *timer) = 0;
};
#endif
