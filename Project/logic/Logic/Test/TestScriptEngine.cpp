#include "TestScriptEngine.h"
#include "Tools.h"
#include "Tools_time.h"
#include "IDataInterchange.h"
#include "IScriptEngine.h"
core::IKernel * TestScriptEngine::s_kernel = nullptr;
IScriptEngine * TestScriptEngine::s_scriptEngine = nullptr;

void TestScriptEngine::Test(core::IKernel *kernel)
{
	s_kernel = kernel;
	//FIND_MODULE(s_scriptEngine, ScriptEngine);
	//s_scriptEngine->RegModuleFunc("Test", "Sub", MyMin, "test.sub");
	//s_scriptEngine->RegModuleFunc("Test", "CallSum", Sum, "test.CallSum");
	//TestCallLuaFunc();
	//TestLoopCallFunc();
}

void TestScriptEngine::TestCallLuaFunc()
{
	s_scriptEngine->PrintLuaStack();
	IDataInterchangeCaller *caller = s_scriptEngine->PrepareCall("hello", "test");
	IDataOutputStream *ostream = caller->GetOutputStream();
	ostream->WriteInt32(1);
	ostream->WriteInt32(2);
	caller->Call(s_kernel, nullptr);
	s_scriptEngine->PrintLuaStack();
}

void TestScriptEngine::TestLoopCallFunc()
{
	IKernel *kernel = s_kernel;
	TRACE_LOG("TestLoopCallFunc");
	s32 a = 4;
	auto inputFun = [a](IKernel *kernel, IDataOutputStream &output)
	{
		output.WriteInt32(a);
	};
	s32 computer = 0;
	auto readFun = [&computer](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(computer);
	};
	s_scriptEngine->Call("npc.xuping", "OnSum", inputFun, readFun);
	
	TRACE_LOG("Test A:%d, LoopSum:%d", a, computer);
}

void TestScriptEngine::MyMin(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out)
{
	static s32 deep = 0;
	deep++;
	TRACE_LOG("Enter Min");
	s_scriptEngine->PrintLuaStack();
	s32 a = 0; 
	s32 b = 0;
	input.ReadInt32(a);
	input.ReadInt32(b);
	TRACE_LOG("A:%d, B:%d", a, b);
	if (deep >= 5)
	{
		TRACE_LOG("^^^^^^^^^^^Return^^^^^^^^^^^^^^");
		out.WriteInt32(0);
	}
	else
	{
		auto inputFun = [a, b](IKernel *kernel, IDataOutputStream &output)
		{
			output.WriteInt32(a+1);
			output.WriteInt32(b+1);
		};
		s32 computer = 0;
		auto readFun = [&computer](IKernel *kernel, IDataInputStream &input)
		{
			input.ReadInt32(computer);
		};
		TRACE_LOG("MinCall, NextStack");
		s_scriptEngine->Call("npc.xuping", "OnStart", inputFun, readFun);
		TRACE_LOG("Computer:%d", computer);
		out.WriteInt32(computer);
		s_scriptEngine->PrintLuaStack();
	}
	
}

void TestScriptEngine::Sum(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out)
{
	s32 a = 0;
	input.ReadInt32(a);
	auto inputFun = [a](IKernel *kernel, IDataOutputStream &output)
	{
		output.WriteInt32(a - 1);
	};
	s32 computer = 0;
	auto readFun = [&computer](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(computer);
	};
	s_scriptEngine->Call("npc.xuping", "OnSum", inputFun, readFun);
	TRACE_LOG("C++ Sum, A:%d, B:%d", a, computer);
	out.WriteInt32(a+computer);
}