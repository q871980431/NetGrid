#include "LuaInterchange.h"
#include "ScriptEngine.h"
#include "lua.hpp"
LuaInputStream::LuaInputStream(lua_State *state)
{
    _luaState = state;
    _index = 0;
    _count = lua_gettop(_luaState);
}
IDataInputStream * LuaInputStream::ReadBool(bool &val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        int tmp = lua_toboolean(_luaState, ++_index);
        val = (tmp == 0)?false : true;
    }
    return this;
}

IDataInputStream * LuaInputStream::ReadInt8(s8 &val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        LUA_INTEGER tmp = lua_tointeger(_luaState, ++_index);
        val = (s8)tmp;
    }

    return this;
}

IDataInputStream * LuaInputStream::ReadInt16(s16 &val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        LUA_INTEGER tmp = lua_tointeger(_luaState, ++_index);
        val = (s16)tmp;
    }
    return this;
}

IDataInputStream * LuaInputStream::ReadInt32(s32 &val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        LUA_INTEGER tmp = lua_tointeger(_luaState, ++_index);
        val = (s32)tmp;
    }

    return this;
}

IDataInputStream * LuaInputStream::ReadInt64(s64 &val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        LUA_INTEGER tmp = lua_tointeger(_luaState, ++_index);
        val = tmp;
    }

    return this;
}

IDataInputStream * LuaInputStream::ReadStr(const char *&val)
{
    ASSERT(_index < _count, "error");
    if (_index < _count)
    {
        val = lua_tostring(_luaState, ++_index);
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
        ASSERT(lua_istable(_luaState, _index + 1), "error");
        lua_getfield(_luaState, _index + 1, PTR_INDEX);
        void *ptr = lua_touserdata(_luaState, -1);
        lua_pop(_luaState, 1);

        lua_getfield(_luaState, _index + 1, TYPE_INDEX);
        const char *luaType = lua_tostring(_luaState, -1);
        lua_pop(_luaState, 1);
        if (strcmp(luaType, type) != 0 || strcmp(type, "") == 0)
        {
            ASSERT(false, "error")
        }
        else{
            val = ptr;
        }

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

void LuaInterchangeCaller::Call(IKernel *kernel, const IDataCallBackFuncType &fun)
{
    _scriptEngine->ExecGlobalFunction(CALL_FUNCTION, _input.Count(), fun);
    s32 indexTop = lua_gettop(_luaState);
    ASSERT(indexTop == _topIndex, "error");
}

