#include "LoginWaitTest.h"
#include "UnitTest.h"
#include "Game_tools.h"
#include "ILoginControl.h"
#include "Tools.h"

UnitTestRegister<LoginWaitTest> loginWaitTest;
s32 LoginWaitTest::s_queueNum = 0;
ILoginControl * LoginWaitTest::s_loginControl;
s32 LoginWaitTest::s_id = 1;
bool LoginWaitTest::s_add = true;

void LoginWaitTest::StartTest(core::IKernel *kernel)
{
	return;
	core::IKernel *s_kernel = kernel;
	FIND_MODULE(s_loginControl, LoginControl);
	StartUpdateQueueNumTimer(kernel);
	StartLoginTimer(kernel);
}

void LoginWaitTest::StartUpdateQueueNumTimer(core::IKernel *kernel)
{
	auto onTimefun = [](core::IKernel *kernel, s64 tick)
	{
		s32 num = 0;
		if (s_queueNum < 300)
		{
			num = tools::GetRandom(1, 20);
			s_add = true;
		}
		else if (s_queueNum < 500)
		{
			if (s_add)
				num = tools::GetRandom(1, 10);
			else
				num = tools::GetRandom(-10, 0);

		}
		else
		{
			num = tools::GetRandom(-10, 0);
			s_add = false;
		}

		s_queueNum += num;
		s_loginControl->UpdateQueueNum(s_queueNum);
	};
	SimpleTimer *timer = NEW SimpleTimer(nullptr, onTimefun, nullptr);
	kernel->StartTimer(timer, 0, FOREVER, 500, "update queue num timer");
}

void LoginWaitTest::StartLoginTimer(core::IKernel *kernel)
{
	auto onTimefun = [](core::IKernel *kernel, s64 tick)
	{
		if (s_id > 5000)
			return;

		s32 num = tools::GetRandom(5, 30);
		char buff[32];
		for (s32 i = 0; i < num; i++)
		{
			s_id++;
			tools::ValToStr(buff, sizeof(buff), s_id);
			s_loginControl->EnterLoginQueue(std::string(buff), s_id);
		}
		s_loginControl->UpdateQueueNum(s_queueNum);
	};
	SimpleTimer *timer = NEW SimpleTimer(nullptr, onTimefun, nullptr);
	kernel->StartTimer(timer, 0, FOREVER, 400, "update queue num timer");
}