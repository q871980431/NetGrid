/*
 * File:	ObjectDes.cpp
 * Author:	xuping
 * 
 * Created On 2017/9/28 16:21:23
 */
#include "ObjectDes.h"
#include "Tools.h"
#include "CommonObject.h"
#include "TableObject.h"
#include "Tools.h"


ObjectDes::ObjectDes(const char *name, const ObjectDes *des):
_name(name), _base(des), _podsSize(0), _allSize(0), _fix(false)
{
    
}

void ObjectDes::AddMember(MemberDes &des)
{
    MemeoryMemberInfo info;
    info.des = des;
	if (info.des.type == DATA_TYPE::DATA_TYPE_TABLE)
	{
		info.des.size = sizeof(TableObject *);
		TableDes *tmp = dynamic_cast<TableDes*>(des.memeoryDes);
		if (tmp == nullptr)
		{
			ASSERT(false, "error");
			return;
		}
		_tables.push_back(info);
	}
	else if (info.des.type == DATA_TYPE::DATA_TYPE_OBJ)
	{
		//_objs.push_back(info);

	}
    else
        _pods.push_back(info);
}

void ObjectDes::Fix()
{
    ASSERT(!_fix, "error");
    if(_base)
    {
        ASSERT(_base->_fix, "error");
        _allSize += _base->_allSize;
    }

    _podstart = _allSize;
    for (auto &pod : _pods)
    {
        pod.offset = _podstart + _podsSize;

        _podsSize += pod.des.size;
        if (pod.des.type == DATA_TYPE::DATA_TYPE_STR)
            _podsSize += 1;
    }
    _allSize += _podsSize;

    //for (auto &member : _objs)
    //{
    //    member.offset = _allSize;
    //    _allSize += member.des.memeoryDes->_allSize;
    //}

    s32 i = 0;
    for (auto &table : _tables)
        table.offset = _allSize + (sizeof(TableObject *) * i++);
    _allSize += (_tables.size() * sizeof(TableObject *));

#ifdef _DEBUG
	_allSize += sizeof(MemeoryTrace);
#endif // _DEBUG
    _allSize += 1;

    _fix = true;
	InitPropertyMap();
}

void ObjectDes::GetProperty(PropertyLists &propertys)
{
    auto f = [&propertys, this](MemeoryMemberInfo &info)
    {
        MemberProperty member;
        member.host = this;
        member.name = info.des.name;
        member.type = info.des.type;
        member.offset = info.offset;
        member.size = info.des.size;
        member.des = &info.des;

        propertys.push_back(member);
    };
    for (auto& pod : _pods)
        f(pod);

    for (auto& obj : _objs)
    {
        f(obj);
        PropertyLists tmp;
        obj.des.memeoryDes->GetProperty(tmp);
        for (auto tmpIter : tmp)
        {
            MemberName name(obj.des.name);
            name << mem_split_name << tmpIter.name;
            tmpIter.name = name;
            tmpIter.offset += obj.offset;

            propertys.push_back(tmpIter);
        }
    }

    for (auto& table : _tables)
    {
        f(table);
        PropertyLists tmp;
		TableDes *tmpDes = (TableDes*)table.des.memeoryDes;
		const char *keyName = tmpDes->GetKeyName();
        table.des.memeoryDes->GetProperty(tmp);
        for (auto tmpIter : tmp)
        {
            MemberName name(table.des.name);
            name << mem_split_name << tmpIter.name;
			if (keyName != nullptr && (strcmp(keyName, tmpIter.name.GetString()) == 0))
			{
				if (tmpIter.type < DATA_TYPE::DATA_TYPE_BOOL || tmpIter.type > DATA_TYPE::DATA_TYPE_STR)
				{
					ASSERT(false, "error");
					continue;
				}
				//tmpDes->SetKeyMemProperty(tmpIter);
			}
			tmpIter.name = name;
			propertys.push_back(tmpIter);
        }
    }
}

void * ObjectDes::CreateMemeory(s32 &size)const
{
    ASSERT(_fix, "don't fix");
    char *addr = NEW char[_allSize];
    InitMemeory(addr, _allSize);
    size = _allSize;
    return addr;
}

void ObjectDes::DestoryMemeory(void *memeory) const
{
    ReleaseMemeory(memeory, _allSize);
    DEL[] memeory;
}

void ObjectDes::ReleaseMemeory(void *addr, s32 size) const
{
    ASSERT(_allSize == size, "size error");
    ASSERT(_fix, "need fix");

    char *addrTmp = (char *)addr;
    const ObjectDes *base = _base;
    if (base)
    {
        base->ReleaseMemeory(addrTmp, base->_allSize);
        addrTmp += base->_allSize;
    }

    ASSERT(addrTmp + _podsSize - (char*)addr <= size, "error");
    addrTmp += _podsSize;

    //for (auto obj : _objs)
    //{
    //    ASSERT(addrTmp + obj.des.memeoryDes->_allSize - addr <= size, "error");
    //    obj.des.memeoryDes->ReleaseMemeory(addrTmp, obj.des.memeoryDes->_allSize);
    //    addrTmp += obj.des.memeoryDes->_allSize;
    //}

    if (_tables.size() > 0)
    {
        for (auto table : _tables)
        {
            ASSERT(addrTmp + sizeof(TableObject*) - (char*)addr <= size, "error");
			TableObject *tableMem = *(TableObject**)addrTmp;
			DEL tableMem;
            addrTmp += sizeof(TableObject*);
        }
    }

#ifdef _DEBUG
	MemeoryTrace *head = (MemeoryTrace *)addrTmp;
	ASSERT(head->size == _allSize, "error");
	ASSERT(head->name == _name.GetString(), "error");
	ASSERT(head->des == this, "error");
	addrTmp += sizeof(MemeoryTrace);
#endif

    ASSERT(addrTmp[0] == 1, "error");
}

void ObjectDes::InitMemeory(void *addr, s32 size) const
{
    ASSERT(_allSize == size, "size error");
    ASSERT(_fix, "need fix");
    char *addrTmp = (char *)addr;

    const ObjectDes *base = _base;
    if(base)
    {
        base->InitMemeory(addrTmp, base->_allSize);
        addrTmp += base->_allSize;
    }

    s32 offset = addrTmp - (char *)addr;
    ASSERT(offset + _podsSize <= size, "error");
    tools::SafeMemset(addrTmp, size - offset, 0, _podsSize);
    addrTmp += _podsSize;

    for (auto &obj : _objs)
    {
        ASSERT(addrTmp + obj.des.memeoryDes->_allSize - (char *)addr <= size, "error");
        obj.des.memeoryDes->InitMemeory(addrTmp, obj.des.memeoryDes->_allSize);
        addrTmp += obj.des.memeoryDes->_allSize;
    }

    if (_tables.size() > 0)
    { 
        for (auto &table : _tables)
        {
            ASSERT(addrTmp + sizeof(TableObject*) - (char *)addr <= size, "error");
			TableObject *tableMem = BuildTableObject(table);
			TableObject **tableObjectPtr = (TableObject **)addrTmp;
			*tableObjectPtr = tableMem;
			//memcpy(addrTmp, tableMem, sizeof(TableObject*));
            addrTmp += sizeof(TableObject*);
        }
    }

#ifdef _DEBUG
	MemeoryTrace *head = (MemeoryTrace *)addrTmp;
	head->des = this;
	head->name = _name.GetString();
	head->size = _allSize;
	addrTmp += sizeof(MemeoryTrace);
#endif

    addrTmp[0] = 1;
    ASSERT(addrTmp + 1 - (char *)addr == size, "error");
}

TableObject * ObjectDes::BuildTableObject(const MemeoryMemberInfo &tableInfo) const
{
	TableDes *tableDes = (TableDes*)tableInfo.des.memeoryDes;
	TableObject *ret = nullptr;
	const auto *keyMemProperty = tableDes->GetKeyMemProperty();
	if (keyMemProperty == nullptr)
		return NEW TableObject(tableDes);
	else if (keyMemProperty->type >= DATA_TYPE::DATA_TYPE_BOOL && keyMemProperty->type <= DATA_TYPE::DATA_TYPE_FLOAT)
	{
		if (tableDes->GetDes())
			return NEW TableObectInt64Key(tableDes, tools::Desc<s64>);
		else
			return NEW TableObectInt64Key(tableDes, tools::Asc<s64>);
	}
	else if (keyMemProperty->type == DATA_TYPE::DATA_TYPE_STR)
	{
		if (tableDes->GetDes())
			return NEW TableObectStrKey(tableDes, tools::Desc<std::string>);
		else
			return NEW TableObectStrKey(tableDes, tools::Asc<std::string>);
	}
	return ret;
}

void ObjectDes::InitPropertyMap()
{
	auto f = [this](MemeoryMemberInfo &info)
	{
		MemberProperty &member = *NEW MemberProperty();
		member.host = this;
		member.name = info.des.name;
		member.type = info.des.type;
		member.offset = info.offset;
		member.size = info.des.size;
		member.des = &info.des;
		this->_propertyMaps.insert(std::make_pair(member.name.GetString(), &member));
	};
	for (auto& pod : _pods)
		f(pod);

	for (auto& obj : _objs)
	{
		f(obj);
		PropertyLists tmp;
		obj.des.memeoryDes->GetProperty(tmp);
		for (auto tmpIter : tmp)
		{
			MemberName name(obj.des.name);
			name << mem_split_name << tmpIter.name;
			tmpIter.name = name;
			tmpIter.offset += obj.offset;
		}
	}

	for (auto& table : _tables)
	{
		f(table);
		PropertyLists tmp;
		TableDes *tmpDes = (TableDes*)table.des.memeoryDes;
		const char *keyName = tmpDes->GetKeyName();

		const auto &tablePropertyMap =  table.des.memeoryDes->GetPropertyMap();
		for (auto &tmpIter : tablePropertyMap)
		{
			MemberName name(table.des.name);
			name << mem_split_name << tmpIter.first.c_str();
			if (keyName != nullptr && (strcmp(keyName, tmpIter.first.c_str()) == 0))
			{
				if (tmpIter.second->type < DATA_TYPE::DATA_TYPE_BOOL || tmpIter.second->type > DATA_TYPE::DATA_TYPE_STR)
				{
					ASSERT(false, "error");
					continue;
				}
				tmpDes->SetKeyMemProperty(tmpIter.second);
			}
			_propertyMaps.insert(std::make_pair(name.GetString(), tmpIter.second));
		}
	}
}