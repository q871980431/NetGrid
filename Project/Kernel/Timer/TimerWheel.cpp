#include "TimerWheel.h"
#include "Tools_time.h"
#include "TimerMgr.h"
#include "../Exception/ExceptionMgr.h"

using namespace tlib::linear;
tlib::TDynPool<TimerBase> TimerBase::s_pool;

void TimerBase::Exec(core::IKernel *kernel, s64 tick)
{
	_canRelease = false;
	if (_frist)
	{
		TRY_BEGIN
		_timer->OnStart(kernel, tick);
		TRY_END
		_frist = false;
	}
	else
	{
		TRY_BEGIN
		_timer->OnTime(kernel, tick);
		TRY_END
	}
	_canRelease = true;
	if (0 != _count)
	{
		if (_count != FOREVER)
			--_count;
		_expires = _wheel->GetJiffies() + _interval;
        _wheel->AddTimer(this);
	}
	else
	{
		End(kernel, tick, false);
	}
}

void TimerBase::Kill(core::IKernel *kernel, s64 tick)
{
	_count = 0;
	if (!_remove)
		End(kernel, tick, true);
}

void TimerBase::End(core::IKernel *kernel, s64 tick, bool isKill)
{
	if (!_remove)
	{
		_remove = true;
		_timer->SetBase(nullptr);
		TRY_BEGIN
		_timer->OnTerminate(kernel, tick, isKill);
		TRY_END
		_host->Remove(this);
	}
	if (_canRelease)
		Release();
}

void TimerWheel::AddTimer(TimerBase *timer, s64 delay)
{
	if (timer->GetTimerWheel() != nullptr)
	{
		ASSERT(false, "error");
		return;
	}
	timer->SetTimerWheel(this);
	timer->SetExpires(_jiffies + delay / JIFFIES_TIME);

	AddTimer(timer);
	return;
}

#define INDEX(N) ((_jiffies >> (TVR_BITS + (N)*TVN_BITS)) & TVN_MASK)

void TimerWheel::Process(core::IKernel *kernel, s32 tick)
{
	s64 now = tools::GetTimeMillisecond();

	auto jiffies = GetJiffies(now);
	while (jiffies > _jiffies)
	{
		LinkList workList;
		s32 index = _jiffies & TVR_MASK;
		if (!index &&
			(!Cascade(_tvec_node + 0, INDEX(0))) &&
			(!Cascade(_tvec_node + 1, INDEX(1))) &&
			!Cascade(_tvec_node + 2, INDEX(2))
		)
			Cascade(_tvec_node + 3, INDEX(3));
		++_jiffies;
		workList.Swap(_tvec_root.vec + index);
		ILinkNode *tmp = workList.HeadRemove();
		while (tmp != nullptr)
		{
			((TimerBase *)tmp)->Exec(kernel, now);
			tmp = workList.HeadRemove();
		}
	}
}

s32 TimerWheel::Cascade(TVEC_NODE *tvec, s32 index)
{
	LinkList *temp = tvec->vec + index;
	ILinkNode *node = temp->HeadRemove();
	while (node != nullptr)
	{
		AddTimer((TimerBase *)node);
		node = temp->HeadRemove();
	}

	return index;
}

void TimerWheel::AddTimer(TimerBase *timer)
{
	JiffiesT expires = timer->GetExpires();
	JiffiesT idx = expires - _jiffies;
	LinkList *vec;

	if (idx < TVR_SIZE) {
		s32 i = expires & TVR_MASK;
		vec = _tvec_root.vec + i;
	}
	else if (idx < 1 << (TVR_BITS + TVN_BITS)) {
		s32 i = (expires >> TVR_BITS) & TVN_MASK;
		vec = _tvec_node[0].vec + i;
	}
	else if (idx < 1 << (TVR_BITS + 2 * TVN_BITS)) {
		s32 i = (expires >> (TVR_BITS + TVN_BITS)) & TVN_MASK;
		vec = _tvec_node[1].vec + i;
	}
	else if (idx < 1 << (TVR_BITS + 3 * TVN_BITS)) {
		s32 i = (expires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK;
		vec = _tvec_node[2].vec + i;
	}
	else if ((signed long)idx < 0) {
		/*
		* Can happen if you add a timer with expires == jiffies,
		* or you set a timer to go off in the past
		*/
		vec = _tvec_root.vec + (_jiffies & TVR_MASK);
	}
	else {
		s32 i;
		if (idx > 0xFFFFFFFFUL)
		{
			idx = 0xFFFFFFFFUL;
			expires = idx + _jiffies;
		}
		i = (expires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK;
		vec = _tvec_node[3].vec + i;
	}
	vec->TailInsert(timer);
}