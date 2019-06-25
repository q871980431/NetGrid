#include "LuaInterchange.h"
#include "ScriptEngine.h"
#include "lua.hpp"
LuaInputStream::LuaInputStream(lua_State *state, bool callStack /* =false */)
{
    _luaState = state;
    _index = 0;
	if (!callStack)
	{
		_start = 0;
		_count = lua_gettop(_luaState);
	}
	else
	{
		_count = 0;
		for (s32 i = 0; i < LUA_RESULT_COUNT; i++)
		{
			if (lua_isnil(_luaState, -LUA_RESULT_COUNT+i))
				break;
			_count++;
		}
		_start = lua_gettop(_luaState) - LUA_RESULT_COUNT;
	}
}

IDataInputStream * LuaInputStream::ReadBool(bool &val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        int tmp = lua_toboolean(_luaState, ++_index+_start);
        val = (tmp == 0)?false : true;
    }
    return this;
}

IDataInputStream * LuaInputStream::ReadInt8(s8 &val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        LUA_INTEGER tmp = lua_tointeger(_luaState, ++_index+_start);
        val = (s8)tmp;
    }

    return this;
}

IDataInputStream * LuaInputStream::ReadInt16(s16 &val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        LUA_INTEGER tmp = lua_tointeger(_luaState, ++_index+_start);
        val = (s16)tmp;
    }
    return this;
}

IDataInputStream * LuaInputStream::ReadInt32(s32 &val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        LUA_INTEGER tmp = lua_tointeger(_luaState, ++_index+_start);
        val = (s32)tmp;
    }

    return this;
}

IDataInputStream * LuaInputStream::ReadInt64(s64 &val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        LUA_INTEGER tmp = lua_tointeger(_luaState, ++_index+_start);
        val = tmp;
    }

    return this;
}

IDataInputStream * LuaInputStream::ReadStr(const char *&val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        val = lua_tostring(_luaState, ++_index+_start);
		lua_len(_luaState, _index+_start);
		size_t len = lua_tointeger(_luaState, -1);
		lua_pop(_luaState, 1);
		ASSERT(val[len] == 0, "error");
		ASSERT(strlen(val) <= len, "error");
    }

    return this;
}

IDataInputStream * LuaInputStream::ReadBlob(const char *&val, s32 &size)
{
    ASSERT(false, "error");

    return this;
}

IDataInputStream * LuaInputStream::ReadPtr(void *&val, const char *type)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        ASSERT(lua_istable(_luaState, _index + 1+_start), "error");
        lua_getfield(_luaState, _index + 1+_start, PTR_INDEX);
        void *ptr = lua_touserdata(_luaState, -1);
        lua_pop(_luaState, 1);

        lua_getfield(_luaState, _index + 1+_start, TYPE_INDEX);
        const char *luaType = lua_tostring(_luaState, -1);
        lua_pop(_luaState, 1);
        if (strcmp(luaType, type) != 0 || strcmp(type, "") == 0)
        {
            ASSERT(false, "error")
        }
        else{
            val = ptr;
        }
		_index++;
    }
    return this;
}

IDataInputStream * LuaInputStream::Reset()
{
    _index = 0;
    return this;
}

LuaOutputStream::LuaOutputStream(lua_State *state)
{
    _luaState = state;
    _count = 0;
}


IDataOutputStream * LuaOutputStream::WriteBool(bool val)
{
    lua_pushboolean(_luaState, val ? 1 : 0);
    ++_count;
    return this;
}

IDataOutputStream * LuaOutputStream::WriteInt8(s8 val)
{
    lua_pushinteger(_luaState, val);
    ++_count;
    return this;
}

IDataOutputStream * LuaOutputStream::WriteInt16(s16 val)
{
    lua_pushinteger(_luaState, val);
    ++_count;
    return this;
}

IDataOutputStream * LuaOutputStream::WriteInt32(s32 val)
{
    lua_pushinteger(_luaState, val);
    ++_count;
    return this;
}

IDataOutputStream * LuaOutputStream::WriteInt64(s64 val)
{
    lua_pushinteger(_luaState, val);
    ++_count;
    return this;
}

IDataOutputStream * LuaOutputStream::WriteStr(const char *val)
{
    lua_pushstring(_luaState, val);
    ++_count;
    return this;
}

IDataOutputStream * LuaOutputStream::WriteBlob(const char *val, s32 size)
{
    ASSERT(false, "error");
    return this;
}

IDataOutputStream * LuaOutputStream::WritePtr(const void *val, const char *type)
{
    lua_createtable(_luaState, 0, 2);
    lua_pushlightuserdata(_luaState, (void *)val);
    lua_setfield(_luaState, -2, PTR_INDEX);
    lua_pushstring(_luaState, type);
    lua_setfield(_luaState, -2, TYPE_INDEX);
    ++_count;
    return this;
}

bool LuaInterchangeCaller::PreCall(const char *module, const char *func)
{
    ASSERT(_use == false, "error");
    if (_use == false)
    {
        _input.Reset();
        _topIndex = lua_gettop(_luaState);

        _trace.Assign(module);
        _trace << "." << func;
        _input.WriteStr(module);
        _input.WriteStr(func);

        _use = true;
    }
    return true;
}

void LuaInterchangeCaller::InitCall(const char *module, const char *func)
{
	_topIndex = lua_gettop(_luaState);

	_trace.Assign(module);
	_trace << "." << func;
	_input.WriteStr(module);
	_input.WriteStr(func);
}

void LuaInterchangeCaller::Call(IKernel *kernel, const IDataCallBackFuncType &fun)
{
	IKernel *_kernel = kernel;
	TRACE_LOG("BeginCall, NextStack");
	_scriptEngine->PrintLuaStack();
    _scriptEngine->ExecGlobalFunction(CALL_FUNCTION, _input.Count(), fun);
    s32 indexTop = lua_gettop(_luaState);
    ASSERT(indexTop == _topIndex, "error");
	if (indexTop != _topIndex)
	{
		IKernel *_kernel = kernel;
		TRACE_LOG("Call lua function error");
	}

	_use = false;
}

