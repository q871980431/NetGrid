#ifndef __Tools_time_h__
#define __Tools_time_h__
#include "MultiSys.h"
#include <chrono>

#define GAME_FRESH_TIME  5
namespace tools
{
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

#ifdef __cplusplus
    extern "C"{
#endif
		//Thread Safe timeFormat = nullʱ�� Ĭ�ϸ�ʽΪ"%4d-%02d-%02d %02d:%02d:%02d"
        const char * GetCurrentTimeString(const char *timeFormat = nullptr);
        bool UpdateLocalTime(time_t time);
        time_t GetYearTime(s32 year, s32 month, s32 day, s32 hour, s32 min);
        s32 GetGameWeek(time_t tick);
#ifdef __cplusplus
    }
#endif
}

#endif
