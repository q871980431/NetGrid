#ifndef __TimerWheel_h__
#define __TimerWheel_h__
#include "MultiSys.h"

class TimerGear
{
public:
protected:
private:
};

class TimerWheel
{
    enum 
    {
        CONFIG_BASE_SMALL = false,
        TVN_BITS = (CONFIG_BASE_SMALL ? 4 : 6),  
        TVR_BITS = (CONFIG_BASE_SMALL ? 6 : 8), 
        TVN_SIZE = (1 << TVN_BITS),  
        TVR_SIZE = (1 << TVR_BITS),  
        TVN_MASK = (TVN_SIZE - 1),  
        TVR_MASK =(TVR_SIZE - 1) 
    };
public:
    TimerWheel(s64 tick) :_tick(tick),_jiffies(0), _next_timer(0){};
protected:
private:
    s64 _tick;
    u32 _jiffies;
    u32 _next_timer;
};
#endif