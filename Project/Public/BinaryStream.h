#ifndef __BinaryStream_h__
#define __BinaryStream_h__
#include "MultiSys.h"

class BinaryStream
{
public:
    virtual ~IStream(){};

    virtual IStream & operator << (bool val) = 0;
    virtual IStream & operator << (s8 val) = 0;
    virtual IStream & operator << (s16 val) = 0;
    virtual IStream & operator << (s32 val) = 0;
    virtual IStream & operator << (s64 val) = 0;
    virtual IStream & operator << (const char *contents) = 0;
    virtual IStream & Write(const void *data, s32 len) = 0;

    virtual IStream & operator >> (bool &val) = 0;
    virtual IStream & operator >> (s8 &val) = 0;
    virtual IStream & operator >> (s16 &val) = 0;
    virtual IStream & operator >> (s32 &val) = 0;
    virtual IStream & operator >> (s64 &val) = 0;
    virtual IStream & operator >> (const char **value) = 0;
    virtual IStream & Read(const void **data, s32 &len) = 0;
};
#endif