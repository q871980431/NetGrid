#ifndef __TimerTest_h__
#define __TimerTest_h__
#include "IUnitTest.h"
class TimerTest : public IUnitTestInstance
{
public:
	virtual void StartTest(core::IKernel *kernel);
protected:
private:
	//core::IKernel *_kernel;
};
#endif
