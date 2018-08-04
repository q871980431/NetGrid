/*
 * File:	LuaTest.cpp
 * Author:	Xuping
 * 
 * Created On 2018/6/23 0:04:33
 */

#include "LuaTest.h"
LuaTest * LuaTest::s_self = nullptr;
IKernel * LuaTest::s_kernel = nullptr;
bool LuaTest::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	write_line(1, 2, 3, 4);
	debugLog(1, 2, 3, 4);
    return true;
}

bool LuaTest::Launched(IKernel *kernel)
{

    return true;
}

bool LuaTest::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


