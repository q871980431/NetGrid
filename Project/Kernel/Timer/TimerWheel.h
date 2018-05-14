#ifndef __TimerWheel_h__
#define __TimerWheel_h__
#include "MultiSys.h"
#include "IKernel.h"
#include "TString.h"
#include "TList.h"
#include "TDynPool.h"
#include "Tools_time.h"

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
	void End(core::IKernel *kernel, s64 tick);

	inline void SetExpires(u32 expires) { _expires = expires; };
	inline u32 GetExpires() { return _expires; };
	inline TimerWheel * GetTimerWheel() { return _wheel; };
	inline void SetTimerWheel(TimerWheel *timerWheel) { _wheel = timerWheel; };
private:
	TimerBase(core::ITimer *timer, s32 count, s32 interval, const char *trace) :_trace(trace),_expires(0)
		,_wheel(nullptr), _frist(true), _kill(false){
		_timer = timer;
		_count = count;
		_interval = interval;
	};
private:
	core::ITimer *_timer;
	u32 _expires;
	TimerWheel *_wheel;

	s32 _count;
	s32 _interval;
	bool _frist;
	bool _kill;
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
    TimerWheel() :_jiffies(0), _next_timer(0){
		_tick = tools::GetTimeMillisecond();
	};

	void AddTimer(TimerBase *timer, s32 expires);
	void Process( core::IKernel *kernel, s32 tick);
    inline u32 GetJiffies(){ return _jiffies; };
private:
	void AddTimer(TimerBase *timer);
	s32 Cascade(TVEC_NODE *tvec, s32 index);
	inline u32 GetJiffies(s64 tick) { return (u32)(tick - _tick) / JIFFIES_TIME; };
protected:
private:
    s64 _tick;
    u32 _jiffies;
    u32 _next_timer;

	TVEC_ROOT	_tvec_root;
	TVEC_NODE	_tvec_node[TVEC_COUNT];
};
#endif