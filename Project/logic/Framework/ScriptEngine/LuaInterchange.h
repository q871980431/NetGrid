#ifndef __LuaInterchange_h__
#define __LuaInterchange_h__
#include "IDataInterchange.h"
#include "TString.h"

static const char *PTR_INDEX = "ptr";
static const char *TYPE_INDEX = "type";
static const char *CALL_FUNCTION = "call";
class lua_State;
class ScriptEngine;
class LuaInputStream : public IDataInputStream
{
public:
    LuaInputStream(lua_State *state);
    virtual ~LuaInputStream(){};

    virtual IDataInputStream * ReadBool(bool &val);
    virtual IDataInputStream * ReadInt8(s8 &val);
    virtual IDataInputStream * ReadInt16(s16 &val);
    virtual IDataInputStream * ReadInt32(s32 &val);
    virtual IDataInputStream * ReadInt64(s64 &val);
    virtual IDataInputStream * ReadStr(const char *&val);
    virtual IDataInputStream * ReadBlob(const char *&val, s32 &size);
    virtual IDataInputStream * ReadPtr(void *&val, const char *type);
    virtual IDataInputStream * Reset();
protected:
private:
    lua_State   *_luaState;
    s32         _index;
    s32         _count;
};

class LuaOutputStream : public IDataOutputStream
{
public:
    LuaOutputStream(lua_State *state);
    virtual ~LuaOutputStream(){};

    virtual IDataOutputStream * WriteBool(bool val);
    virtual IDataOutputStream * WriteInt8(s8 val);
    virtual IDataOutputStream * WriteInt16(s16 val);
    virtual IDataOutputStream * WriteInt32(s32 val);
    virtual IDataOutputStream * WriteInt64(s64 val);
    virtual IDataOutputStream * WriteStr(const char *val);
    virtual IDataOutputStream * WriteBlob(const char *val, s32 size);
    virtual IDataOutputStream * WritePtr(const void *val, const char *type);
    inline s32 Count(){ return _count; };
    inline void Reset(){ _count = 0; };
protected:
private:
    lua_State   *_luaState;
    s32         _count;
};

class LuaInterchangeCaller : public IDataInterchangeCaller
{
public:
    LuaInterchangeCaller(ScriptEngine *scriptEngine, lua_State *state) : _scriptEngine(scriptEngine), 
    _input(state){
        _use = false;
        _luaState = state;
    };
    virtual ~LuaInterchangeCaller(){};

    virtual IDataOutputStream * GetOutputStream(){ return &_input; };
    virtual void Call(IKernel *kernel, const IDataCallBackFuncType &fun);
public:
    bool PreCall(const char *module, const char *func);
private:
    LuaOutputStream     _input;
    s32                 _topIndex;
    tlib::TString<128>  _trace;
    ScriptEngine        *_scriptEngine;
    bool                 _use;
    lua_State            *_luaState;
};

#endif
