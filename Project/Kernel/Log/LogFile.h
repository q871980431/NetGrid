#ifndef __LogFile_h__
#define __LogFile_h__
#include "MultiSys.h"
class LogFile
{
public:
    LogFile() :_file(nullptr),_createTick(0){};
    ~LogFile(){ Close(); };

    bool Open(const char *path, const char *name);
    void Close();
    void Write(const char *contents);
    void Write(const char *data, s32 len);
    void Flush();

    inline bool IsOpen(){ return nullptr != _file; };
    inline s64  CreateTick(){ return _createTick; };
private:
    FILE    *_file;
    s64     _createTick;
};
#endif