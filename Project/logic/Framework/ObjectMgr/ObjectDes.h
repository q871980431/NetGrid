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
#include <map>

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
class ObjectDes;
class TableObject;

struct MemberDes 
{
    MemberName name;
    s32 index;
    s16 type;
    s32 size;
    ObjectDes *memeoryDes;
};

struct MemberProperty  : public IMember
{
    ObjectDes *host;
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
	const ObjectDes *des;
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
    const MemberDes   *des;
};

typedef std::vector<MemberProperty> PropertyLists;
class ObjectDes
{
public:
    typedef std::vector<MemeoryMemberInfo> MemberSlots;
	typedef std::map<std::string, MemberProperty*> PropertyMap;

    ObjectDes(const char *name, const ObjectDes *des);
	virtual ~ObjectDes() {};

    void AddMember(MemberDes &des);
    void Fix();
    void GetProperty(PropertyLists &propertys);
    const char * GetMemeoryName() const { return _name.GetString(); };
	const PropertyMap & GetPropertyMap() { return _propertyMaps; };

public:
    void *  CreateMemeory(s32 &size) const;
    void    DestoryMemeory(void *memeory) const;
    s32 GetMemorySize() const { ASSERT(_fix, "error"); return _allSize; };

public:
	template <typename T>
	void SetAttrT(void *addr, const MemberProperty *member, T val)const;	
	template <typename T>
	T GetAttrT(void *addr, const MemberProperty *member)const;

	inline void SetAttrBlob(void *addr, const MemberProperty *member, void *val, s32 size) const
	{
		ASSERT(member->host == this, "error");
		ASSERT(member->type == DATA_TYPE::DATA_TYPE_BINARY, "error");
		ASSERT(member->size >= size, "error");
		ASSERT(member->offset + size <= _allSize, "error");
		if (member->size < size)
			size = member->size;
		memcpy((char *)addr + member->offset, val, size);
		*(s32*)((char*)addr + member->offset + member->size) = size;
	}

	inline void * GetAttrBlob(void *addr, const MemberProperty *member, s32& size) const
	{
		ASSERT(member->host == this, "error");
		ASSERT(member->type == DATA_TYPE::DATA_TYPE_BINARY, "error");
		ASSERT(member->offset + member->size <= _allSize, "error");
		size = *(s32*)((char*)addr + member->offset + member->size);
		return (char*)addr + member->offset;
	}

	inline void * GetAttrData(void *addr, const MemberProperty *member, s32 size) const
	{
		ASSERT(member->host == this, "error");
		ASSERT(member->offset + member->size <= _allSize, "error");
		ASSERT(member->size == size, "error");
		if (member->host == this && member->size == size)
			return (char*)addr + member->offset;
		return nullptr;
	}
private:
    inline void SetAttrStruct(void *addr, const MemberProperty *member, void *conext, s32 size)const
    {
        ASSERT(member->offset + size <= _allSize, "error");
        if (member->size == size)
            memcpy((char *)addr + member->offset, conext, size);
    }

	template <typename T>
	T * GetAttrTPtr(void *addr, const MemberProperty *member)const
	{
		ASSERT(member->size == sizeof(T*), "error");
		return (T *)((char *)addr + member->offset);
	}
    void * GetChildObj(void *addr, const MemberProperty *member)const
    {
        return (void *)((char *)addr + member->offset);
    }
public:    
protected:
    void InitMemeory(void *addr, s32 size) const;
    void ReleaseMemeory(void *address, s32 size) const;
	TableObject * BuildTableObject(const MemeoryMemberInfo &tableInfo)const;
	void InitPropertyMap();
private:
    MemeoryName              _name;
    const ObjectDes        *_base;
    MemberSlots              _objs;
    MemberSlots              _tables;
    MemberSlots              _pods;
    s32                      _podstart;
    s32                      _podsSize;
    s32                      _allSize;
    bool                     _common;
    bool                     _fix;

	PropertyMap				 _propertyMaps;
};

template <typename T>
void ObjectDes::SetAttrT(void *addr, const MemberProperty *member, T val)const
{
	ASSERT(member->host == this, "error");
	SetAttrStruct(addr, member, &val, sizeof(T));
}

template<>
inline void ObjectDes::SetAttrT<const char *>(void *addr, const MemberProperty *member, const char * val) const
{
	printf("setAttr string, filename:%s", member->name.GetString());
	ASSERT(member->host == this, "error");
	ASSERT(member->type == DATA_TYPE::DATA_TYPE_STR, "error");
	s32 size = strlen(val);
	if (member->size < size)
		size = member->size;
	ASSERT(member->offset + size <= _allSize, "error");
	memcpy((char *)addr + member->offset, val, size);
	((char*)addr)[member->offset + size] = 0;
}

template <typename T>
T ObjectDes::GetAttrT(void *addr, const MemberProperty *member)const
{
	void *ret = GetAttrData(addr, member, sizeof(T));
	return ret ? *(T*)ret : T();
}

template<>
inline const char * ObjectDes::GetAttrT<const char *>(void *addr, const MemberProperty *member) const
{
	printf("GetAttr string, filename:%s", member->name.GetString());
	ASSERT(member->type == DATA_TYPE::DATA_TYPE_STR, "error");
	return(char *)addr + member->offset;
}


class TableDes : public ObjectDes
{
public:
	TableDes(const char *name) :ObjectDes(name, nullptr),_keyMemProperty(nullptr), _des(false) {};
	virtual ~TableDes() { };

	inline void SetKeyName(const char *name) { _keyName = name; };
	inline const char * GetKeyName() { return _keyName.c_str(); };
	inline void SetKeyMemProperty(const MemberProperty  *memberProperty) { 
		_keyMemProperty = memberProperty; };
	inline const MemberProperty * GetKeyMemProperty() const { return _keyMemProperty; };
	inline void SetDes(bool des) { _des = des; };
	inline bool GetDes() { return _des; };
private:
	std::string _keyName;
	const MemberProperty *_keyMemProperty;
	bool _des;
};
#endif