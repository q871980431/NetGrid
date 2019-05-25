#ifndef __AsyncTest_h__
#define __AsyncTest_h__
#include "IUnitTest.h"
class AsyncTest : public IUnitTestInstance
{
public:
	virtual void StartTest(core::IKernel *kernel);

protected:
private:
	void TestKernelAsync(core::IKernel *kernel);
	void TestAsyncQueue(core::IKernel *kernel);
};
#endif
