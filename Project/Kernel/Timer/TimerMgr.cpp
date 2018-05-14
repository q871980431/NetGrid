#include "TimerMgr.h"
#include "../Kernel.h"

bool TimerMgr::Ready()
{
	_kernel = KERNEL;
	return true;
}
void TimerMgr::Process(s32 tick)
{
	_timerWheel.Process(_kernel, tick);
}

void TimerMgr::StartTimer(core::ITimer *timer, s32 delay, s32 count, s32 interval, const char *trace)
{
	ASSERT(timer->GetBase() == nullptr, "error");
	TimerBase *base = TimerBase::Create(timer, count, interval, trace);
	timer->SetBase(base);

	_timerWheel.AddTimer(base, delay);
}

void TimerMgr::KillTimer(core::ITimer *timer)
{
	ASSERT(timer->GetBase() != nullptr, "error");
	TimerBase *base = (TimerBase *)timer->GetBase();
	s64 tick = tools::GetTimeMillisecond();
	base->Kill(_kernel, tick);
}