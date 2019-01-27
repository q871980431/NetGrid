/*
 * File:	ObjectDes.h
 * Author:	xuping
 * 
 * Created On 2017/9/28 16:21:23
 */

#ifndef __ObjectDes_h__
#define __ObjectDes_h__
#include "MultiSys.h"
#include "TString.h"
#include "IObjectMgr.h"
#include "TCallBack.h"
#include <vector>
#include <unordered_map>

const static s16 MEMBERNAME_LENGTH = 256;
const static char *bool_name = "bool";
const static char *s8_name = "s8";
const static char *s16_name = "s16";
const static char *s32_name = "s32";
const static char *s64_name = "s64";
const static char *float_name = "float";
const static char *str_name = "str";
const static char *binary_name = "blob";
const static char *table_name = "array";
const static char *space_split_name = "::";
const static char *mem_split_name = "::";
const static char *des_extension_name = "xml";
const static char *init_namespace = "";

typedef tlib::TString<MEMBERNAME_LENGTH> MemberName;
typedef MemberName  TableName;
typedef MemberName MemeoryName;
class MemeoryDes;
struct MemberDes 
{
    MemberName name;
    s32 index;
    s16 type;
    s32 size;
    MemeoryDes *memeoryDes;
};

struct MemberProperty  : public IMember
{
    MemeoryDes *host;
    MemberName name;
    s32 offset;
    s32 size;
    s16 type;
    MemberDes *des;
};

struct MemeoryMemberInfo
{
    MemberDes des;
    s32 offset;
};

struct MemeoryTrace 
{
    const char *name;
    s32 size;
};

struct Memeroy 
{
    void *addr;
    s32 size;
};

struct TableMember 
{
    std::vector<Memeroy> members;
    std::unordered_map<s64, Memeroy *> index;
    const MemberDes   *des;
};

typedef std::vector<MemberProperty> PropertyLists;
class MemeoryDes
{
public:
    typedef void(*MEMBER_ONCHANGE_CB)(void *object, const MemberProperty *member, void *context, s32 size);
    typedef tlib::TCallBack<const MemberProperty *, MEMBER_ONCHANGE_CB, void *, const MemberProperty *, void *, s32> MemberChangeCallPool;

    typedef std::vector<MemeoryMemberInfo> MemberSlots;
    MemeoryDes(const char *name, const MemeoryDes *des, bool common);
	~MemeoryDes() {};

    void AddMember(MemberDes &des);
    void Fix();
    void GetProperty(PropertyLists &propertys);
    const char * GetMemeoryName() const { return _name.GetString(); };

public:
    void *  CreateMemeory(s32 &size);
    void    DestoryMemeory(void *memeory) const;
    s32 GetMemorySize() const { ASSERT(_fix, "error"); return _allSize; };

	IObject * CreateObject();
	void	DestoryObject(IObject *obj) const;
	inline IObject * GetObject(void *ptr) {
		char *tmpObj = (char*)ptr + sizeof(MemeoryDes **);
		return (IObject *)((char*)ptr + sizeof(MemeoryDes **));
	};
public:
    void RegisterMemberChangeCallBack(void *object, const MemberProperty *member, MEMBER_ONCHANGE_CB callBack, const char *debug);
    void UnRegisterMemberChangeCallBack(void *object, const MemberProperty *member, MEMBER_ONCHANGE_CB callBack);
	inline MemberChangeCallPool * GetCallPoolPtr(void *addr) const;
public:
    template <typename T>
    void SetAttrT(void *addr, const MemberProperty *member, T val)const
    {
        SetAttrStruct(addr, member, &val, sizeof(T));
    }

    inline void SetAttrStruct(void *addr, const MemberProperty *member, void *conext, s32 size)const
    {
        const MemeoryDes **des = (const MemeoryDes **)((char*)addr);
        ASSERT(member->offset + size <= (*des)->_allSize, "error");
        if (member->size == size)
        {
            memcpy((char *)addr + member->offset, conext, size);
            if (!(*des)->_common)
            {
                MemberChangeCallPool *pool = GetCallPoolPtr(addr);
                pool->Call(member, addr, member, conext, size);
                pool->Call(nullptr, addr, member, conext, size);
            }
        }
    }

    template <typename T>
    T GetAttrT(void *addr, const MemberProperty *member)const
    {
        return *(T *)((char *)addr + member->offset);
    }

    void * GetChildObj(void *addr, const MemberProperty *member)const
    {
        return (void *)((char *)addr + member->offset);
    }
public:
    static inline Memeroy * TableAddRow(TableMember *table)
    {
        Memeroy memeroy;
        memeroy.addr = table->des->memeoryDes->CreateMemeory(memeroy.size);
        ASSERT(memeroy.addr != nullptr, "error");
        table->members.push_back(memeroy);

        return &(table->members.back());
    }

    inline void ReleaseTable(TableMember *table) const
    {
        for (auto member : table->members)
            table->des->memeoryDes->ReleaseMemeory(member.addr, member.size);
    }

    static inline s16 RowCount(void *addr)
    {
        TableMember *table = (TableMember *)addr;
        return (s16)(table->members.size());
    }

    inline Memeroy *GetRow(void *addr, s16 index)const
    {
        TableMember *table = (TableMember *)addr;
        ASSERT(index < table->members.size(), "error");
        return &table->members[index];
    }

    static inline Memeroy *CreateRow(void *addr)
    {
        TableMember *table = (TableMember *)addr;
        return TableAddRow(table);
    }
    
    inline void CreateIndex(void *addr, s64 key, Memeroy *memeroy)const
    {
        auto ret = ((TableMember *)addr)->index.insert(std::make_pair(key, memeroy));
        ASSERT(ret.second, "error");
    }
    

protected:
    void InitMemeory(void *addr, s32 size) const;
    void ReleaseMemeory(void *address, s32 size) const;
private:
    MemeoryName              _name;
    const MemeoryDes        *_base;
    MemberSlots              _objs;
    MemberSlots              _tables;
    MemberSlots              _pods;
    s32                      _podstart;
    s32                      _podsSize;
    s32                      _allSize;
    bool                     _common;
    bool                     _fix;
};

class CommonObject : public IObject
{
public:
	CommonObject(void *data) :_data(data) {};
	virtual ~CommonObject() {};

	virtual s32 GetMemberS32(const IMember *member) { return GetAttrT<s32>(member); };
	virtual s64 GetMemberS64(const IMember *member) { return GetAttrT<s64>(member); };

	virtual void SetMemberS32(const IMember *member, s32 val) { SetAttrT<s32>(member, val); };
	virtual void SetMemberS64(const IMember *member, s64 val) { SetAttrT<s64>(member, val); };;

	virtual ITable  * GetTable(const IMember *member);
public:
	inline void Release() {
		const MemeoryDes **des = (const MemeoryDes **)(_data);
		(*(const MemeoryDes **)(_data))->DestoryObject(this);
	};
	inline void * GetData() { return _data; };

	template< typename T>
	inline T GetAttrT(const IMember *member)
	{
		const MemeoryDes **des = (const MemeoryDes **)(_data);
		return (*des)->GetAttrT<T>(_data, (const MemberProperty *)member);
	}
	template< typename T>
	inline void SetAttrT(const IMember *member, T val)
	{
		const MemeoryDes **des = (const MemeoryDes **)(_data);
		(*des)->SetAttrT<T>(_data, (const MemberProperty *)member, val);
	}

protected:
private:
	void *_data;
};

class TableObject : public ITable
{
public:
	TableObject( TableMember *tableMember) :_tableMember(tableMember){};
	~TableObject() {};

	IObject * TableAddRow(TableMember *table)
	{
		Memeroy memeroy;
		memeroy.addr = table->des->memeoryDes->CreateMemeory(memeroy.size);
		ASSERT(memeroy.addr != nullptr, "error");
		table->members.push_back(memeroy);
		return table->des->memeoryDes->GetObject(memeroy.addr);
	}

	inline IObject *GetRow(s16 index)const
	{
		ASSERT(index < _tableMember->members.size(), "error");
		if (index < _tableMember->members.size())
			_tableMember->des->memeoryDes->GetObject(&_tableMember->members[index].addr);

		return nullptr;
	}

	IObject * FindRowByKey(s16 key)
	{
		return nullptr;
	}

private:
	TableMember	*_tableMember;
};
#endif