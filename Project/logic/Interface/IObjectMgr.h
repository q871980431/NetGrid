/*
 * File:	IObjectMgr.h
 * Author:	xuping
 * 
 * Created On 2017/9/28 16:21:23
 */

#ifndef  __IObjectMgr_h__
#define __IObjectMgr_h__
#include "IModule.h"
#define ANY nullptr
//typedef void CommonObject;

class IMember
{
public:
    virtual ~IMember(){};
};

class IRow
{
public:
    virtual ~IRow(){};

    virtual s32 GetMemberS32(IMember *member) = 0;
protected:
private:
};

class IObject;
class ITable
{
public:
    virtual ~ITable(){};

    virtual IObject * CreateRow() = 0;
    virtual IObject * GetRow(s16 index) = 0;
    virtual IObject * FindRowByKey(s16 key) = 0;
};

class IObject
{
public:
    virtual ~IObject(){};

    virtual s32 GetMemberS32(const IMember *member) = 0;
    virtual s64 GetMemberS64(const IMember *member) = 0;
	virtual void SetMemberS32(const IMember *member, s32 val) = 0;
	virtual void SetMemberS64(const IMember *member, s64 val) = 0;
    virtual ITable  * GetTable(const IMember *member) = 0;
};

const static s64 INVALIED_ID = -1;
class IObjectMgr : public IModule
{
public:
    virtual ~IObjectMgr(){};

    virtual IObject * CreateObject(const char *fullName) = 0;
    //virtual s16  GetAttrInt16(void *object, const MemberProperty *member) = 0;
    //virtual void SetAttrInt16(void *object, const MemberProperty *member, s16 val) = 0;
    //virtual s32  GetAttrInt32(void *object, const MemberProperty *member) = 0;
    //virtual void SetAttrInt32(void *object, const MemberProperty *member, s32 val) = 0;
    //virtual s64  GetAttrInt64(void *object, const MemberProperty *member) = 0;
    //virtual void SetAttrInt64(void *object, const MemberProperty *member, s64 val) = 0;
};

#endif