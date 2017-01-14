#ifndef __IReader_h__
#define __IReader_h__
#include "MultiSys.h"
class INameReader
{
public:
    virtual ~INameReader(){};

    virtual s16 GetCount() = 0;
    virtual s8  GetS8(const char *key) = 0;
    virtual s16 GetS16(const char *key) = 0;
    virtual s32 GetS32(const char *key) = 0;
    virtual s64 GetS64(const char *key) = 0;
    virtual const char * GetStr(const char *key) = 0;
    virtual void * GetBinary(const char *key, s32 &size) = 0;
    virtual INameReader * GetReader(const char *key) = 0;
};

#endif