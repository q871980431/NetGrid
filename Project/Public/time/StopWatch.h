#ifndef __COMMON_TIME_STOPWATCH_H__
#define __COMMON_TIME_STOPWATCH_H__
#include "MultiSys.h"
#include <chrono>
template<typename T = std::chrono::milliseconds>
class StopWatch
{
public:
    StopWatch(){Reset();}
    inline void Reset(){ _tick = std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count();}
    inline s64  Interval(){ return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch()).count() - _tick;}
    void Printf( const char * content = nullptr){
        s64 time = Interval();
        if (nullptr != content)
        {
            ECHO("%s interval time = %ld ms", content, time);
        }
        else
        {
            ECHO("interval time = %ld ms", time);
        }
    }
protected:
private:
    s64 _tick;
};
#endif