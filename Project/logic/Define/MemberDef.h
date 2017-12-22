/*
 * File:	MemberDef.h
 * Author:	xuping
 * 
 * Created On 2017/12/16 22:06:09
 */

#ifndef __MemberDefine_h__
#define __MemberDefine_h__
#ifdef WIN32
    #ifdef MEMBER_EXPORT
        #define MEMBER_API __declspec(dllexport)
    #else
        #define MEMBER_API __declspec(dllimport)
    #endif
#else
    #define MEMBER_API
#endif

class IMember;

struct MEMBER_API Root
{
    static const IMember * id;
};

struct MEMBER_API Logic
{
    struct MEMBER_API Root
    {
        static const IMember * id;
    };

    struct MEMBER_API Wing
    {
        static const IMember * wingid;
        static const IMember * order;
    };

    struct MEMBER_API Player
    {
        static const IMember * lvl;
        static const IMember * bag;
        static const IMember * wingbag;

        struct MEMBER_API Bag
        {
            static const IMember * place;
            static const IMember * randattr;

            struct MEMBER_API Randattr
            {
                static const IMember * id;
                static const IMember * type;
                static const IMember * value;
            };

        };

        struct MEMBER_API Wingbag
        {
            static const IMember * wing;

            struct MEMBER_API Wing
            {
                static const IMember * wingid;
                static const IMember * order;
            };

        };

    };

};

#endif