#ifndef __IDataInterchange_h__
#define __IDataInterchange_h__
#include "IKernel.h"
using namespace core;

class IDataInputStream
{
public:
    virtual ~IDataInputStream(){};
    virtual IDataInputStream * ReadBool(bool &val) = 0;
    virtual IDataInputStream * ReadInt8(s8 &val) = 0;
    virtual IDataInputStream * ReadInt16(s16 &val) = 0;
    virtual IDataInputStream * ReadInt32(s32 &val) = 0;
    virtual IDataInputStream * ReadInt64(s64 &val) = 0;
    virtual IDataInputStream * ReadStr(const char *&val) = 0;
    virtual IDataInputStream * ReadBlob(const char *&val, s32 &size) = 0;
    virtual IDataInputStream * ReadPtr(void *&val, const char *type) = 0;
    virtual IDataInputStream * Reset() = 0;
};

class IDataOutputStream
{
public:
    virtual ~IDataOutputStream(){};
    virtual IDataOutputStream * WriteBool(bool val) = 0;
    virtual IDataOutputStream * WriteInt8(s8 val) = 0;
    virtual IDataOutputStream * WriteInt16(s16 val) = 0;
    virtual IDataOutputStream * WriteInt32(s32 val) = 0;
    virtual IDataOutputStream * WriteInt64(s64 val) = 0;
    virtual IDataOutputStream * WriteStr(const char *val) = 0;
    virtual IDataOutputStream * WriteBlob(const char *val, s32 size) = 0;
    virtual IDataOutputStream * WritePtr(const void *val, const char *type) = 0;
};

typedef std::function<void(IKernel *kernel, IDataInputStream &input)> IDataCallBackFuncType;
typedef std::function<void(IKernel *kernel, IDataInputStream &input, IDataOutputStream &out)> IDataInterchangeFuncType;

class IDataInterchangeCaller
{
public:
    virtual ~IDataInterchangeCaller(){};
    virtual IDataOutputStream * GetOutputStream() = 0;
    virtual void Call(IKernel *kernel, const IDataCallBackFuncType &fun) = 0;
};

#endif