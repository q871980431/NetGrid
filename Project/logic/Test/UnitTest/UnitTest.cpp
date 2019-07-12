/*
 * File:	UnitTest.cpp
 * Author:	XuPing
 * 
 * Created On 2019/5/16 16:19:20
 */

#include "UnitTest.h"
UnitTest * UnitTest::s_self = nullptr;
IKernel * UnitTest::s_kernel = nullptr;
GLOBAL_VAR_INT_MAX TestList UnitTest::s_testList;

bool UnitTest::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool UnitTest::Launched(IKernel *kernel)
{
	TRACE_LOG("UnitTest 111111");
	printf("TestList Size,%ld", s_testList.size());
	for (auto &test : s_testList)
	{
		test->StartTest(kernel);
	}

    return true;
}

bool UnitTest::Destroy(IKernel *kernel)
{
	for (auto test : s_testList)
	{
		DEL test;
	}
	s_testList.clear();
    DEL this;
    return true;
}


