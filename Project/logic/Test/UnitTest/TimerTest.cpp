#include "TimerTest.h"
#include "UnitTest.h"
#include "Game_tools.h"
#include "Tools_time.h"

const static UnitTestRegister<TimerTest> test;
void TimerTest::StartTest(core::IKernel *kernel)
{
	static const s32 timerCount = 20;
	static SimpleTimer *timers[timerCount];

	TRACE_LOG("Enter Start Test");
	for (s32 i = 0; i < timerCount; i++)
	{
		auto startFun = [i](core::IKernel *kernel, s64 tick)
		{
			TRACE_LOG("timer:%d, start fun", i);
		};
		auto timeFun = [i](core::IKernel *kernel, s64 tick)
		{
			TRACE_LOG("timer:%d, onTime fun", i);
			if (i %3 == 0)
			{
				TRACE_LOG("timer:%d, exec kill", i);
				kernel->KillTimer(timers[i]);
				timers[i] = nullptr;
			}
		};

		auto onTerminateFun = [i](core::IKernel *kernel, s64 tick, bool isKill)
		{
			TRACE_LOG("timer:%d, onTerminate fun, Kill:%d", i, isKill);
		};
		SimpleTimer *timer = NEW SimpleTimer(startFun, timeFun, onTerminateFun);
		timers[i] = timer;
		if (i < 10)
		{
			ADD_TIMER(timer, i * 1000, 10, 1000);
		}
		else
		{
			s64 delay = tools::DAY * 2 * 365;
			ADD_TIMER(timer, delay + i * 1000, 10, 1000);
		}
	}
}
