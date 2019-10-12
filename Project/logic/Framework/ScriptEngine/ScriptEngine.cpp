/*
 * File:	ScriptEngine.cpp
 * Author:	xuping
 * 
 * Created On 2017/8/1 21:33:16
 */

#include "ScriptEngine.h"
#include "lua.hpp"
#include "LuaInterchange.h"
ScriptEngine * ScriptEngine::s_self = nullptr;
IKernel * ScriptEngine::s_kernel = nullptr;
lua_State * ScriptEngine::s_luaState = nullptr;
LuaInterchangeCaller * ScriptEngine::s_caller = nullptr;

bool ScriptEngine::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;

    s_luaState = luaL_newstate();
    luaL_openlibs(s_luaState);
    s_caller = NEW LuaInterchangeCaller(s_self, s_luaState);
  
    char scriptPath[MAX_PATH];
    const char *envi = s_kernel->GetEnvirPath();
    SafeSprintf(scriptPath, sizeof(scriptPath), "%s/script", envi);
	ECHO("LUA PATH:%s", scriptPath);
    SetSearchPath(scriptPath);
    ExecScriptFile(LINK_FILE_NAME);

    ReplacePrint();
    //lua_getglobal(s_luaState, LUA_TRACE_NAME);


    return true;
}

bool ScriptEngine::Launched(IKernel *kernel)
{
	ECHO("Script Engine Launched***************");
    s_self->RegModuleFunc("test", "TestAdd", TestAdd, "**************");
	s_self->RegModuleFunc("test", "TestFunc", TestFunc, "**************");
	s_self->RegModuleFunc("test", "TestNest", TestNest, "**************");
	
    //s_self->RegModuleFunc("test", "TestAdd", TestAdd, "**************");

    //IDataInterchangeCaller *caller = s_self->PrepareCall("npc.xuping", "OnStart");
    //IDataOutputStream *ostream = caller->GetOutputStream();
    //ostream->WriteInt32(1);
    //ostream->WriteInt32(2);
    //caller->Call(s_kernel, nullptr);

	//caller = s_self->PrepareCall("npc.xuping", "Computer");
	//ostream = caller->GetOutputStream();
	//ostream->WriteInt32(10);
	//caller->Call(s_kernel, nullptr);

	auto f = [](IKernel *kernel, IDataOutputStream &output)
	{
		//s32 *p = NEW s32();
		//*p = 5;
		//output.WritePtr(p, "s32");
		output.WriteInt32(3);
	};
	s32 ret = 0;
	auto callFun = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(ret);
	};
	CallScriptFunc("npc.xuping", "testNest", f, callFun);

    return true;
}

bool ScriptEngine::Destroy(IKernel *kernel)
{
    DEL s_caller;
    lua_close(s_luaState);
    DEL this;
    return true;
}

IDataInterchangeCaller * ScriptEngine::PrepareCall(const char *module, const char * func)
{
    s_caller->PreCall(module, func);
    return s_caller;
}

void ScriptEngine::Call(const char *module, const char * func, const IDataOutputFuncType &writeFun, const IDataCallBackFuncType &readFun)
{
	LuaInterchangeCaller caller(s_self, s_luaState);
	caller.InitCall(module, func);
	if (writeFun)
	{
		IDataOutputStream *outStream = caller.GetOutputStream();
		writeFun(s_kernel, *outStream);
	}
	caller.Call(s_kernel, readFun);
}

void ScriptEngine::RegModuleFunc(const char *module, const char *func, const IDataInterchangeFuncType &f, const char *debug)
{
    char path[MAX_PATH];
    SafeSprintf(path, sizeof(path), "serverd.%s", module);

    lua_getglobal(s_luaState, PACKAGE);                 // pkg
    lua_getfield(s_luaState, -1, PKG_MODULE_INDEX);     // pkg modules 
    lua_pushstring(s_luaState, path);                   // pkg modules path
    lua_rawget(s_luaState, -2);                         // pkg modules moudules[path]
    if (lua_isnil(s_luaState, -1))
    {
        lua_pop(s_luaState, 1);
        lua_pushstring(s_luaState, path);               // pkg modules path
        lua_newtable(s_luaState);                 // pkg modules path table
        lua_rawset(s_luaState, -3);                     // pkg modules

        lua_pushstring(s_luaState, path);
        lua_rawget(s_luaState, -2);
    }
    // pkg modules modules[path]
    lua_pushstring(s_luaState, func);
    void *userdata = lua_newuserdata(s_luaState, sizeof(IDataInterchangeFuncType));
    IDataInterchangeFuncType *funType = NEW(userdata)IDataInterchangeFuncType();
    *funType = f;

    lua_pushcclosure(s_luaState, LuaCall, 1);
    lua_rawset(s_luaState, -3);
    lua_pop(s_luaState, 3);
}

void ScriptEngine::CallScriptFunc(const char *module, const char *func, const IDataOutputFuncType &outPutFunc, const IDataCallBackFuncType &callBackFun)
{
	//s32 top = lua_gettop(s_luaState);
	LuaInterchangeCaller caller(this, s_luaState);
	//top = lua_gettop(s_luaState);
	caller.PreCall(module, func);
	if (outPutFunc != nullptr)
	{
		IDataOutputStream *outStream = caller.GetOutputStream();
		outPutFunc(s_kernel, *outStream);
	}
	caller.Call(s_kernel, callBackFun);
}

s32 ScriptEngine::CallLuaFuncAdd(s32 a, s32 b)
{
	lua_pushstring(s_luaState, "npc.xuping");
	lua_pushstring(s_luaState, "Add");
	lua_pushinteger(s_luaState, a);
	lua_pushinteger(s_luaState, b);
	s32 ret = 0;
	auto func = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(ret);
	};
	ExecGlobalFunction(CALL_FUNCTION, 4, func);
	return ret;
}

void ScriptEngine::TestCallLuaFunction()
{
    lua_getglobal(s_luaState, "test");
    lua_pushnumber(s_luaState, 1);
    lua_pushnumber(s_luaState, 2);
    lua_call(s_luaState, 2, 0);
}

void ScriptEngine::SetSearchPath(const char *path)
{
    lua_getglobal(s_luaState, PACKAGE);
    lua_getfield(s_luaState, -1, PKG_PATH_INDEX);
    const char *cur_path = lua_tostring(s_luaState, -1);
    ECHO("CUR_PATH:%s", cur_path);
    char buff[1024];
    //SafeSprintf(buff, sizeof(buff), "%s;%s/?.lua", cur_path, path);
    SafeSprintf(buff, sizeof(buff), "%s/?.lua", path);
    lua_pushstring(s_luaState, buff);
    lua_setfield(s_luaState, -3, PKG_PATH_INDEX);
    lua_pop(s_luaState, 2);
}

bool ScriptEngine::ExecScriptFile(const char *file)
{
    char buff[512];
    SafeSprintf(buff, sizeof(buff), "return require \"%s\"", file);
    luaL_loadstring(s_luaState, buff);

    return ExecFunction(0, nullptr);
}

int ScriptEngine::LuaCall(lua_State *state)
{
    IDataInterchangeFuncType func = *(IDataInterchangeFuncType *)lua_touserdata(s_luaState, lua_upvalueindex(1));
    ASSERT(func != nullptr, "error");
    LuaInputStream input(s_luaState);
    LuaOutputStream output(s_luaState);
    func(s_kernel, input, output);

    return output.Count();
}

int ScriptEngine::Log(lua_State *state)
{
	ECHO("Enter Script Engine Log");
    tlib::TString<4096> content;
    luaL_where(state, 1);
    content << lua_tostring(state, -1);
    lua_pop(state, 1);
    switch (lua_type(state, 1))
    {
    case LUA_TNIL: content << "nil"; break;
    case LUA_TNUMBER:
    {
        if (lua_isinteger(state, 1))
            content << (s64)lua_tointeger(state, 1);
        else
            content << (float)lua_tonumber(state, 1);
    }
        break;
    case LUA_TBOOLEAN:content << ((lua_toboolean(state, 1) ? "true" : "false")); break;
    case LUA_TSTRING: content << lua_tostring(state, 1); break;
    default:content << luaL_typename(state, 1) << ":" << (s64)lua_topointer(state, 1);break;
    }
    core::IKernel *kernel = s_kernel;
    LUA_LOG(content.GetString());
    return 0;
}

bool ScriptEngine::ExecGlobalFunction(const char *func, s8 argc, const IDataCallBackFuncType callback)
{
	IKernel *kernel = s_kernel;
    lua_getglobal(s_luaState, func);				//将func 函数压入栈中
    if (!lua_isfunction(s_luaState, -1))
    {
        ASSERT(false, "error, don't find function: %s", func);
        lua_pop(s_luaState, 1);
        return false;
    }
	TRACE_LOG("************Exe ExecGlobalFunction**************");
	PrintLuaStack();
    if (argc > 0)
        lua_insert(s_luaState, -(argc + 1));		//将func 移动到 距离栈顶 argc+1的位置
	TRACE_LOG("************Exe ExecGlobalFunction Insert**************");
	PrintLuaStack();
    return ExecFunction(argc, callback);
}

void ScriptEngine::PrintLuaStack()
{
	IKernel *kernel = s_kernel;
	lua_State* pLuaState = s_luaState;
	s32 stackTop = lua_gettop(pLuaState);//获取栈顶的索引值
	TRACE_LOG(" element count: %d", stackTop); 
	TRACE_LOG("--栈顶(v)(%d)--", stackTop);
	//显示栈中的元素	
	for(s32 nIdx = stackTop; nIdx > 0; --nIdx)
	{		
		s32 nType = lua_type(pLuaState, nIdx);
		switch (nType)
		{
		case LUA_TBOOLEAN:
		case LUA_TNUMBER: {
			if (lua_isinteger(pLuaState, nIdx))
			{
				s64 val = lua_tonumber(pLuaState, nIdx);
				TRACE_LOG("(i:%d) %s(%ld)", nIdx, lua_typename(pLuaState, nType), val);
			}
			else
			{
				TRACE_LOG("(i:%d) %s(%f)", nIdx, lua_typename(pLuaState, nType), lua_tonumber(pLuaState, nIdx));
			}

		}
			break;
		default:
			TRACE_LOG("(i:%d) %s(%s)", nIdx, lua_typename(pLuaState, nType), lua_tostring(pLuaState, nIdx));
		}
	} 
	TRACE_LOG("--栈底--");
}

bool ScriptEngine::ExecFunction(s8 argc, const IDataCallBackFuncType callback)
{
    core::IKernel *kernel = s_kernel;
    s8 findex = -(argc + 1);
    if (!lua_isfunction(s_luaState, findex))
    {
        lua_pop(s_luaState, -findex);
        return false;
    }
    s32 tarceIndex = 0;
    lua_getglobal(s_luaState, LUA_TRACE_NAME);
    if (!lua_isfunction(s_luaState, -1))
    {
        lua_pop(s_luaState, 1);
    }
    else
    {
        tarceIndex = findex - 1;
        lua_insert(s_luaState, tarceIndex);
    }
	TRACE_LOG("************Exe PCallBegin**************");
	PrintLuaStack();
    s32 error = lua_pcall(s_luaState, argc, LUA_RESULT_COUNT, tarceIndex);
    if (error != LUA_OK)
    {
        if (tarceIndex == 0)
        {
            ERROR_LOG("Lua error:%s", lua_tostring(s_luaState, -1));
            lua_pop(s_luaState, 1);
        }
        else
            lua_pop(s_luaState, 2);
        return false;
    }
    LuaInputStream ack(s_luaState, true);
    if (callback != nullptr)
        callback(s_kernel, ack);
	TRACE_LOG("************Exe PCallEnd**************");
	PrintLuaStack();
    lua_pop(s_luaState, LUA_RESULT_COUNT);
    if (tarceIndex != 0)
        lua_pop(s_luaState, 1);

    return true;
}

void ScriptEngine::ReplacePrint()
{
    lua_register(s_luaState, "print", Log);
    //lua_pushcclosure(s_luaState, Log, 0);//lua_setglobal(s_luaState,)
}

void ScriptEngine::TestAdd(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out)
{
    s32 a, b;
    input.ReadInt32(a)->ReadInt32(b);
	if (b == 0)
	{
		out.WriteInt32(1);
		return;
	}
	s32 ret = s_self->CallLuaFuncAdd(a, b-1);
    out.WriteInt32(ret);
}

void ScriptEngine::TestFunc(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out)
{
	s32 a;
	input.ReadInt32(a);
	if (a == 0)
	{
		out.WriteInt32(0);
	}
	else
	{
		auto f = [a](IKernel *kernel, IDataOutputStream &output)
		{
			output.WriteInt32(a);
		};
		s32 ret = 0;
		auto callBack = [&ret](IKernel *kernel, IDataInputStream &input)
		{
			input.ReadInt32(ret);
		};
		s_self->CallScriptFunc("npc.xuping", "test", f, callBack);
		out.WriteInt32(ret);
	}
}

void ScriptEngine::TestNest(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out)
{
	s32 tmp = 0;
	//s32 *p = nullptr;
	input.ReadInt32(tmp);
	//input.ReadPtr((void * &)p, "s32");
	if (tmp == 1)
	{
		out.WriteInt32(1);
		return;
	}
	auto f = [tmp](IKernel *kernel, IDataOutputStream &output)
	{
		output.WriteInt32(tmp);
	};
	s32 ret = 0;
	auto callFun = [&ret](IKernel *kernel, IDataInputStream &input)
	{
		input.ReadInt32(ret);
	};
	s_self->CallScriptFunc("npc.xuping", "testNest", f, callFun);

	out.WriteInt32(ret);
}