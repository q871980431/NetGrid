#ifndef __TOOLS_H__
#define __TOOLS_H__
#include "MultiSys.h"
#include <chrono>
#ifdef WIN32
#include <shlwapi.h>
#endif

#ifdef LINUX
#include<libgen.h>
#endif

unsigned int  Radnom();
namespace tools{
    int GetRandom(int nA, int nB);
    inline s64 GetTimeMillisecond(){
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    s32 HashKey(const char *content){
        s32 hash = 131;
        while (*content != '\0')
            hash += hash * 33 + *(content++);
        return hash;
    }

    inline void Mkdir(const char *path)
    {
#ifdef WIN32
        CreateDirectory(path, NULL);
#elif defined LINUX
        mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    }
    //format = "%4d-%02d-%02d %02d:%02d:%02d"
    extern const char *format;
#ifdef __cplusplus
    extern "C"{
#endif
        const char * GetCurrentTimeString();
        const char * GetAppPath();
        bool UpdateLocalTime(time_t time);

#ifdef __cplusplus
    }
#endif



}

#endif 
