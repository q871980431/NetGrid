#ifndef __TestScriptEngine_h__
#define __TestScriptEngine_h__

#include "IKernel.h"
#include "MultiSys.h"
#include <unordered_map>
#include "IDataInterchange.h"
using namespace core;
class IScriptEngine;
class TestScriptEngine
{
public:
	TestScriptEngine() {};
	static void Test(core::IKernel *kernel);

	static void TestCallLuaFunc();
	static void TestLoopCallFunc();


	static void MyMin(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out);
	static void Sum(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out);
private:
	static core::IKernel	*s_kernel;
	static IScriptEngine	*s_scriptEngine;
};

#endif