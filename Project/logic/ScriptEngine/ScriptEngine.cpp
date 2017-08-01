/*
 * File:	ScriptEngine.cpp
 * Author:	xuping
 * 
 * Created On 2017/8/1 21:33:16
 */

#include "ScriptEngine.h"
#include "lua.hpp"
ScriptEngine * ScriptEngine::s_self = nullptr;
IKernel * ScriptEngine::s_kernel = nullptr;
bool ScriptEngine::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool ScriptEngine::Launched(IKernel *kernel)
{
    lua_State* lua_state = luaL_newstate();
    luaL_openlibs(lua_state);
    luaL_dofile(lua_state, "hellolua.lua");
    lua_close(lua_state);

    return true;
}

bool ScriptEngine::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


