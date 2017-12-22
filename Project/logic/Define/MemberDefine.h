#ifndef __MemberDefine_h__
#define __MemberDefine_h__
#include "MultiSys.h"
#ifdef WIN32
    #ifdef MEMBER_EXPORT
        #define MEMBER_API __declspec(dllexport)
    #else
        #define MEMBER_API __declspec(dllimport)
    #endif
#else
    #define MEMBER_API
#endif

struct MEMBER_API Member
{
    static s32 member1;
    static s32 member2;

    struct MEMBER_API Logic
    {
        static s32 member1;
        static s32 member2;
    };
};

#endif