#ifndef __Login_Wait_Test_h__
#define __Login_Wait_Test_h__
#include "IUnitTest.h"

class ILoginControl;
class LoginWaitTest : public IUnitTestInstance
{
public:
	virtual void StartTest(core::IKernel *kernel);

private:
	void StartUpdateQueueNumTimer(core::IKernel *kernel);
	void StartLoginTimer(core::IKernel *kernel);
private:
	static s32 s_queueNum;
	static ILoginControl *s_loginControl;
	static s32 s_id;
	static bool s_add;
};
#endif
