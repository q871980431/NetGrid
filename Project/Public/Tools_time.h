#ifndef __Tools_time_h__
#define __Tools_time_h__
#include "MultiSys.h"
#include <chrono>

#define GAME_FRESH_TIME  5

namespace tools
{
	const static s64 DAY = 86400000;
	const static s64 HOUR = 3600000;
	const static s64 MINUTE = 60000;
	const static s64 MILLISECONDS = 1000;
    extern const char *format;
	inline s64 GetTimeSecond() { return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
    inline s64 GetTimeMillisecond(){ return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
	inline s64 GetTimeNanosecond(){ return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();}
	//Thread Safe
	inline const tm * SafeLocalTime(const time_t *tick)
	{
		static thread_local tm tmp;
#ifdef WIN32
		localtime_s(&tmp, tick);
#endif
#ifdef LINUX
		localtime_r(tick, &tmp);
#endif
		return &tmp;
	}

	inline s32 GetTimeZoneNow()
	{
#ifdef WIN32
		TIME_ZONE_INFORMATION zoneInfo;
		GetSystemTime(&zoneInfo.StandardDate);
		GetTimeZoneInformation(&zoneInfo);
		return zoneInfo.Bias / -60;
#endif
#ifdef LINUX
		return timezone / -3600;
#endif
	}

#ifdef __cplusplus
    extern "C"{
#endif
		//Thread Safe timeFormat = null时， 默认格式为"%4d-%02d-%02d %02d:%02d:%02d"
        const char * GetCurrentTimeString(const char *timeFormat = nullptr);
        bool UpdateLocalTime(time_t time);
        time_t GetYearTime(s32 year, s32 month, s32 day, s32 hour, s32 min);
        s32 GetGameWeek(time_t tick);
#ifdef __cplusplus
    }
#endif
	template<typename T = std::chrono::milliseconds>
	class StopWatch
	{
	public:
		StopWatch() { Reset(); }
		inline void Reset() { _tick = std::chrono::duration_cast<T>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
		inline int64_t  Interval() { return std::chrono::duration_cast<T>(std::chrono::steady_clock::now().time_since_epoch()).count() - _tick; }
	protected:
	private:
		int64_t _tick;
	};
}

#endif
