/*
 * File:	ObjectDes.cpp
 * Author:	xuping
 * 
 * Created On 2017/9/28 16:21:23
 */
#include "ObjectDes.h"
#include "Tools.h"


MemeoryDes::MemeoryDes(const char *name, const MemeoryDes *des, bool common):
_name(name), _base(des), _podsSize(0), _allSize(0), _fix(false), _common(common)
{
    
}

void MemeoryDes::AddMember(MemberDes &des)
{
    MemeoryMemberInfo info;
    info.des = des;
    if (info.des.type == DATA_TYPE::DATA_TYPE_TABLE)
        _tables.push_back(info);
    else if (info.des.type == DATA_TYPE::DATA_TYPE_OBJ)
        _objs.push_back(info);
    else
        _pods.push_back(info);
}

void MemeoryDes::Fix()
{
    ASSERT(!_fix, "error");
    _allSize += sizeof(MemeoryDes **);
	_allSize += sizeof(CommonObject);
    if (!_common)
        _allSize += sizeof(MemberChangeCallPool);

#ifdef _DEBUG
    _allSize += sizeof(MemeoryTrace);
#endif // _DEBUG
    const MemeoryDes *temp = _base;
    while (temp)
    {
        ASSERT(temp->_fix, "error");
        _allSize += temp->_allSize;
        temp = temp->_base;
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
    for (auto &member : _objs)
    {
        member.offset = _allSize;
        _allSize += member.des.memeoryDes->_allSize;
    }
    s32 i = 0;
    for (auto &table : _tables)
        table.offset = _allSize + (sizeof(TableMember) * i++);
    _allSize += (_tables.size() * sizeof(TableMember));
    _allSize += 1;

    _fix = true;
}

void MemeoryDes::GetProperty(PropertyLists &propertys)
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
            tmpIter.host = this;

            propertys.push_back(tmpIter);
        }
    }
    for (auto& table : _tables)
    {
        f(table);
        PropertyLists tmp;
        table.des.memeoryDes->GetProperty(tmp);
        for (auto tmpIter : tmp)
        {
            MemberName name(table.des.name);
            name << mem_split_name << tmpIter.name;
            tmpIter.name = name;
            tmpIter.host = this;
            propertys.push_back(tmpIter);
        }
    }
}

void * MemeoryDes::CreateMemeory(s32 &size)
{
    ASSERT(_fix, "don't fix");
    char *addr = NEW char[_allSize];
    InitMemeory(addr, _allSize);
    size = _allSize;
    return addr;
}

void MemeoryDes::DestoryMemeory(void *memeory) const
{
    ReleaseMemeory(memeory, _allSize);
    DEL[] memeory;
}

IObject * MemeoryDes::CreateObject()
{
	ASSERT(_fix, "don't fix");
	char *addr = NEW char[_allSize];
	InitMemeory(addr, _allSize);
	addr += sizeof(MemeoryDes **);
	CommonObject *commonObj = (CommonObject*)addr;
	return commonObj;
}

void MemeoryDes::DestoryObject(IObject *obj) const
{
	void *memeory = ((CommonObject*)obj)->GetData();
	DestoryMemeory(memeory);
}

void MemeoryDes::ReleaseMemeory(void *addr, s32 size) const
{
    ASSERT(_allSize == size, "size error");
    ASSERT(_fix, "need fix");

    char *addrTmp = (char *)addr;
    addrTmp += sizeof(MemeoryDes **);
	CommonObject *commonObj = (CommonObject*)addrTmp;
	commonObj->~CommonObject();
	addrTmp += sizeof(CommonObject);
    if (!_common)
    {
        MemberChangeCallPool *pool = (MemberChangeCallPool *)addrTmp;
        pool->~TCallBack();
        addrTmp += sizeof(MemberChangeCallPool);
    }

#ifdef _DEBUG
    MemeoryTrace *head = (MemeoryTrace *)addrTmp;
    ASSERT(head->size == _allSize, "error");
    ASSERT(head->name == _name.GetString(), "error");
    addrTmp += sizeof(MemeoryTrace);
#endif

    const MemeoryDes *base = _base;
    if (base)
    {
        base->ReleaseMemeory(addrTmp, base->_allSize);
        addrTmp += base->_allSize;
    }
    ASSERT(addrTmp + _podsSize - addr <= size, "error");
    addrTmp += _podsSize;
    for (auto obj : _objs)
    {
        ASSERT(addrTmp + obj.des.memeoryDes->_allSize - addr <= size, "error");
        obj.des.memeoryDes->ReleaseMemeory(addrTmp, obj.des.memeoryDes->_allSize);
        addrTmp += obj.des.memeoryDes->_allSize;
    }
    if (_tables.size() > 0)
    {
        for (auto table : _tables)
        {
            ASSERT(addrTmp + sizeof(TableMember) - addr <= size, "error");
            TableMember *tableMem = (TableMember *)addrTmp;
            ReleaseTable(tableMem);
            tableMem->~TableMember();

            addrTmp += sizeof(TableMember);
        }
    }
    ASSERT(addrTmp[0] == 1, "error");
}

void MemeoryDes::InitMemeory(void *addr, s32 size) const
{
    ASSERT(_allSize == size, "size error");
    ASSERT(_fix, "need fix");
    char *addrTmp = (char *)addr;
    const MemeoryDes **tmp = (const MemeoryDes **)addrTmp;
    *tmp = this;
    addrTmp += sizeof(MemeoryDes **);
	CommonObject *commonObj = NEW(addrTmp)CommonObject(addr);
	addrTmp += sizeof(CommonObject);
    if (!_common)
    {
        NEW(addrTmp)MemberChangeCallPool();
        addrTmp += sizeof(MemberChangeCallPool);
    }
    const MemeoryDes *base = _base;
#ifdef _DEBUG
    MemeoryTrace *head = (MemeoryTrace *)addrTmp;
    head->name = _name.GetString();
    head->size = _allSize;
    addrTmp += sizeof(MemeoryTrace);
#endif
    if(base)
    {
        base->InitMemeory(addrTmp, base->_allSize);
        addrTmp += base->_allSize;
    }
    s32 offset = addrTmp - addr;
    ASSERT(offset + _podsSize <= size, "error");
    tools::SafeMemset(addrTmp, size - offset, 0, _podsSize);
    addrTmp += _podsSize;

    for (auto &obj : _objs)
    {
        ASSERT(addrTmp + obj.des.memeoryDes->_allSize - addr <= size, "error");
        obj.des.memeoryDes->InitMemeory(addrTmp, obj.des.memeoryDes->_allSize);
        addrTmp += obj.des.memeoryDes->_allSize;
    }

    if (_tables.size() > 0)
    { 
        for (auto &table : _tables)
        {
            ASSERT(addrTmp + sizeof(TableMember) - addr <= size, "error");
            TableMember *tableMem = NEW(addrTmp)TableMember();
            tableMem->des = &(table.des);
            addrTmp += sizeof(TableMember);
        }
    }
    addrTmp[0] = 1;
    ASSERT(addrTmp + 1 - addr == size, "error");
}

void MemeoryDes::RegisterMemberChangeCallBack(void *object, const MemberProperty *member, MEMBER_ONCHANGE_CB callBack, const char *debug)
{
    const MemeoryDes **des = (const MemeoryDes **)((char*)object);
    if (member != nullptr )
    {
        if (*des != member->host)
        {
            ASSERT(false, "error, member %s is not obj %s member", member->name.GetString(), (*des)->_name.GetString());
            return;
        }
    }
    if ((*des)->_common)
    {
        ASSERT(false, "error, obj %s is common object, not bind callback", (*des)->_name.GetString());
        return;
    }
    MemberChangeCallPool *pool = GetCallPoolPtr(object);
    pool->RegisterCallBack(member, callBack, debug);
}

void MemeoryDes::UnRegisterMemberChangeCallBack(void *object, const MemberProperty *member, MEMBER_ONCHANGE_CB callBack)
{
    const MemeoryDes **des = (const MemeoryDes **)((char*)object);
    if (*des != member->host)
    {
        ASSERT(false, "error, member %s is not obj %s member", member->name.GetString(), (*des)->_name.GetString());
        return;
    }
    if ((*des)->_common)
    {
        ASSERT(false, "error, obj %s is common object, not bind callback", (*des)->_name.GetString());
        return;
    }
    MemberChangeCallPool *pool = GetCallPoolPtr(object);
    pool->UnRegisterCallBack(member, callBack);
}

MemeoryDes::MemberChangeCallPool * MemeoryDes::GetCallPoolPtr(void *addr) const
{
	return (MemberChangeCallPool *)((char*)addr + sizeof(CommonObject) + sizeof(const MemeoryDes **));
}

ITable * CommonObject::GetTable(const IMember *member)
{
	return nullptr;
}