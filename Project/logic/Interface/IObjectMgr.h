/*
 * File:	IObjectMgr.h
 * Author:	xuping
 * 
 * Created On 2017/9/28 16:21:23
 */

#ifndef  __IObjectMgr_h__
#define __IObjectMgr_h__
#include "IModule.h"
#include "TCallBack.h"
#include <functional>

#define ANY nullptr
enum ID_TYPE
{
	ID_TYPE_LOCAL = 1,
	ID_TYPE_GUID = 2,
};
//typedef void CommonObject;
class IMember
{
public:
	virtual ~IMember() {};
};

typedef void(*MEMBER_ONCHANGE_CB)(void *object, const IMember *member, void *context, s32 size);
typedef tlib::TCallBack<const IMember *, MEMBER_ONCHANGE_CB, void *, const IMember *, void *, s32> MemberChangeCallPool;
typedef void(*GUID_ALLOCATER_CB)(s64 &guidMin, s32 &num);

class IPod
{
public:
	virtual ~IPod() {};

	//virtual s16 GetMemberS16(const IMember *member) = 0;
	virtual s32 GetMemberS32(const IMember *member) = 0;
	virtual s64 GetMemberS64(const IMember *member) = 0;
	virtual const char * GetMemberStr(const IMember *member) = 0;
	virtual void SetMemberS32(const IMember *member, s32 val) = 0;
	virtual void SetMemberS64(const IMember *member, s64 val) = 0;
	virtual void SetMemberStr(const IMember *member, const char *val) = 0;
};

class IRow : public IPod
{
public:
	virtual ~IRow() {};

	virtual s32 GetIndex() = 0;
};

class ITable
{
public:
	typedef std::function<void(IRow *row)> CallBackFun;
    virtual ~ITable(){};

    virtual IRow * CreateRow() = 0;
    virtual IRow * GetRow(s32 index) = 0;
	virtual s32	   RowCount() = 0;
	virtual void   DelRow(s32 index) = 0;
	virtual void   SwapRow(s32 index, s32 targetIndex) = 0;
	virtual void   ForEach(const CallBackFun &fun) = 0;

public:
	virtual IRow * AddRowByKey(s64 key) = 0;
	virtual IRow * AddRowByKey(const char *key) = 0;
    virtual IRow * FindRowByKey(s64 key) = 0;
	virtual IRow * FindRowByKey(const char *key) = 0;
};

#define DEL_TABLE_ROW(table, row)\
	s32 index = row->GetIndex();\
	s32 lastIndex = table->RowCount() - 1;\
	if(index != lastIndex)\
		table->SwapRow(index, lastIndex);\
	table->DelRow(lastIndex);

class IObject : public IPod
{
public:
    virtual ~IObject(){};

    virtual ITable  * GetTable(const IMember *member) = 0;
};


const static s64 INVALIED_ID = -1;
class IObjectMgr : public IModule
{
public:
    virtual ~IObjectMgr(){};

	virtual void SetGUIDAllocater(const GUID_ALLOCATER_CB &cbFun) = 0;
    virtual IObject * CreateObject(const char *fullName, ID_TYPE type = ID_TYPE_GUID, s64 id = 0) = 0;
	virtual void ReleaseObject(s64 id) = 0;
};

#endif