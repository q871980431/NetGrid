#ifndef __TimerWheel_h__
#define __TimerWheel_h__
#include "MultiSys.h"
#include "IKernel.h"
#include "TString.h"
#include "TList.h"
#include "TDynPool.h"
#include "Tools_time.h"

typedef u64 JiffiesT;
class TimerWheel;
class TimerBase : public core::ITrace, public tlib::linear::ILinkNode
{
	friend class tlib::TDynPool<TimerBase>;
public:
	static TimerBase * Create(core::ITimer *timer, s32 count, s32 interval, const char *trace)
	{
		return CREAT_FROM_POOL(s_pool, timer, count, interval, trace);
	}

	void Release()
	{
		s_pool.Recover(this);
	}
public:
	virtual const char * GetTraceInfo() { return _trace.GetString(); };

	void Exec(core::IKernel *kernel, s64 tick);
	void Kill(core::IKernel *kernel, s64 tick);
	void End(core::IKernel *kernel, s64 tick, bool isKill);

	inline void SetExpires(JiffiesT expires) { _expires = expires; };
	inline JiffiesT GetExpires() { return _expires; };
	inline TimerWheel * GetTimerWheel() { return _wheel; };
	inline void SetTimerWheel(TimerWheel *timerWheel) { _wheel = timerWheel; };
private:
	TimerBase(core::ITimer *timer, s32 count, s32 interval, const char *trace) : _timer(timer),
		_expires(0), _interval(interval),_wheel(nullptr), _count(count),
		_frist(true), _canRelease(true), _remove(false), _trace(trace){
	};
private:
	core::ITimer *_timer;
	JiffiesT	_expires;
	JiffiesT	_interval;
	TimerWheel *_wheel;

	s32 _count;
	bool _frist;
	bool _canRelease;
	bool _remove;
	tlib::TString<TRACE_LEN> _trace;

	static tlib::TDynPool<TimerBase> s_pool;
};

class TimerWheel
{
public:
	enum 
    {
        CONFIG_BASE_SMALL = false,
        TVN_BITS = (CONFIG_BASE_SMALL ? 4 : 6),  
        TVR_BITS = (CONFIG_BASE_SMALL ? 6 : 8), 
        TVN_SIZE = (1 << TVN_BITS),  
        TVR_SIZE = (1 << TVR_BITS),  
        TVN_MASK = (TVN_SIZE - 1),  
        TVR_MASK =(TVR_SIZE - 1),
		TVEC_COUNT = 4,
		JIFFIES_TIME = 10,
    };

	template<s32 N>
	struct TVEC 
	{
		tlib::linear::LinkList vec[N];
	};
	typedef TVEC<TVR_SIZE>	TVEC_ROOT;
	typedef TVEC<TVN_SIZE>	TVEC_NODE;
public:
    TimerWheel() :_jiffies(0){
		_tick = tools::GetTimeMillisecond();
	};

	void AddTimer(TimerBase *timer, s64 delay);
	void AddTimer(TimerBase *timer);
	void Process( core::IKernel *kernel, s32 tick);
    inline JiffiesT GetJiffies(){ return _jiffies; };

private:
	s32 Cascade(TVEC_NODE *tvec, s32 index);
	inline JiffiesT GetJiffies(s64 tick) { return (JiffiesT)(tick - _tick) / JIFFIES_TIME; };

private:
    s64			_tick;
	JiffiesT    _jiffies;

	TVEC_ROOT	_tvec_root;
	TVEC_NODE	_tvec_node[TVEC_COUNT];
};
#endif