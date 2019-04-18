/*
 * File:	MemberDef.h
 * Author:	xuping
 * 
 * Created On 2019/3/27 17:12:44
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

    struct MEMBER_API Player
    {
        static const IMember * name;
        static const IMember * lvl;
        static const IMember * bag;
        static const IMember * skill;
        static const IMember * notice;

        struct MEMBER_API Bag
        {
            static const IMember * place;
        };

        struct MEMBER_API Skill
        {
            static const IMember * skillId;
        };

        struct MEMBER_API Notice
        {
            static const IMember * content;
        };

    };

    struct MEMBER_API Wing
    {
        static const IMember * wingid;
        static const IMember * order;
    };

};

#endif