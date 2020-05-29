#include "AsyncTest.h"
#include "UnitTest.h"
#include "Game_tools.h"

UnitTestRegister<AsyncTest> test;
void AsyncTest::StartTest(core::IKernel *kernel)
{
	return;
	TestKernelAsync(kernel);
	TestAsyncQueue(kernel);
}

void AsyncTest::TestKernelAsync(core::IKernel *kernel)
{
	for (s32 i = 0; i < 10; i++)
	{
		auto asyncExecFun = [i](core::IKernel *kernel, s32 queueId, s32 threadIdx)->bool
		{
			THREAD_LOG("TestKernelAsync", "Simple Task[%d], Async Exec fun, queueId:%d, theadIdx:%d", i, queueId, threadIdx);
			if (i % 2 == 0)
				return false;
			return true;
		};

		auto successFun = [i](core::IKernel *kernel)
		{
			THREAD_LOG("TestKernelAsync", "Simple Task[%d], OnSuccess fun", i);
		};

		auto onFailedFun = [i](core::IKernel *kernel, bool isExecuted)
		{
			THREAD_LOG("TestKernelAsync", "Simple Task[%d], OnSuccess fun, Executed:%d", i, isExecuted);
		};
		SimpleAsyncTask *task = NEW SimpleAsyncTask(asyncExecFun, successFun, onFailedFun);
		kernel->StartAsync(i, task, __FILE__, __LINE__);
	}
}


void AsyncTest::TestAsyncQueue(core::IKernel *kernel)
{
	core::IAsyncQueue *queue = kernel->CreateAsyncQueue(2, __FUNCTION__);
	if (queue)
	{
		for (s32 i = 0; i < 10; i++)
		{
			auto asyncExecFun = [i](core::IKernel *kernel, s32 queueId, s32 threadIdx)->bool
			{
				THREAD_LOG("TestAsyncQueue", "Simple Task[%d], Async Exec fun, queueId:%d, theadIdx:%d", i, queueId, threadIdx);
				if (i % 2 == 0)
					return false;
				return true;
			};

			auto successFun = [i](core::IKernel *kernel)
			{
				THREAD_LOG("TestAsyncQueue", "Simple Task[%d], OnSuccess fun", i);
			};

			auto onFailedFun = [i](core::IKernel *kernel, bool isExecuted)
			{
				THREAD_LOG("TestAsyncQueue", "Simple Task[%d], OnSuccess fun, Executed:%d", i, isExecuted);
			};
			SimpleAsyncTask *task = NEW SimpleAsyncTask(asyncExecFun, successFun, onFailedFun);
			queue->StartAsync(i, task, __FILE__, __LINE__);
		}
	}
}