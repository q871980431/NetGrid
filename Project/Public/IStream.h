#ifndef __IStream_h__
#define __IStream_h__
#include "MultiSys.h"
//class IStream
//{
//public:
//    virtual ~IStream(){};
//
//    virtual IStream & operator << (bool val) = 0;
//    virtual IStream & operator << (s8 val) = 0;
//    virtual IStream & operator << (s16 val) = 0;
//    virtual IStream & operator << (s32 val) = 0;
//    virtual IStream & operator << (s64 val) = 0;
//    virtual IStream & operator << (const char *contents) = 0;
//    virtual IStream & Write(const void *data, s32 len) = 0;
//
//    virtual IStream & operator >> (bool &val) = 0;
//    virtual IStream & operator >> (s8 &val) = 0;
//    virtual IStream & operator >> (s16 &val) = 0;
//    virtual IStream & operator >> (s32 &val) = 0;
//    virtual IStream & operator >> (s64 &val) = 0;
//    virtual IStream & operator >> (const char **value) = 0;
//    virtual IStream & Read(const void **data, s32 &len) = 0;
//};

class IBStream
{
public:
    IBStream(char *data, s32 len) :_data(data), _len(len), _offset(0){};
    IBStream & operator << (s32 val){
        if (_offset + sizeof(val) <= _len)
        {
            memcpy(_data + _offset, &val, sizeof(val));
            _offset += sizeof(val);
        }
        return *this;
    };
    IBStream & Write(s32 val)
    {
        if (_offset + sizeof(val) <= _len)
        {
            *(s32 *)(_data + _offset) = val;
            _offset += sizeof(val);
        }
        return *this;
    }
    inline void Clear(){ _offset = 0; };
protected:
private:
    char *_data;
    s32   _offset;
    s32   _len;
};

class OBStream
{
public:
    OBStream(char *data, s32 len) :_data(data), _len(len), _offset(0){};

    OBStream & operator >> (s32 &val)
    {
        if (_offset + sizeof(s32) <= _len)
        {
            val = *(s32 *)(_data + _offset);
            _offset += sizeof(s32);
        }
    }

protected:
private:
    char *_data;
    s32   _offset;
    s32   _len;
};

template <s32 max = 128>
class FixStream : public IBStream
{
public:
    FixStream() :IBStream(_buff, max){};
protected:
private:
    char _buff[max];
};

#endif