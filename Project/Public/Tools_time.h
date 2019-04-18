#ifndef __Tools_time_h__
#define __Tools_time_h__
#include "MultiSys.h"
#include <chrono>

#define GAME_FRESH_TIME  5
namespace tools
{
    extern const char *format;
    inline s64 GetTimeMillisecond(){
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

	inline s64 GetTimeNanosecond()
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	}

#ifdef __cplusplus
    extern "C"{
#endif
        const char * GetCurrentTimeString();
        bool UpdateLocalTime(time_t time);
        time_t GetYearTime(s32 year, s32 month, s32 day, s32 hour, s32 min);
        s32 GetGameWeek(time_t tick);
#ifdef __cplusplus
    }
#endif
}

#endif
