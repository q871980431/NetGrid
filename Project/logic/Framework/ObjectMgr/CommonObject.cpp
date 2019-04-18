#include "CommonObject.h"
#include "TableObject.h"

bool CommonObject::Init(s64 GUID)
{
	_memeroy.addr = _des->CreateMemeory(_memeroy.size);
	if (_memeroy.addr)
	{
		_GUID = GUID;
		_callPool = NEW MemberChangeCallPool();
		return true;
	}

	return false;
}

ITable * CommonObject::GetTable(const IMember *member)
{
	TableObject *table = GetAttrT<TableObject*>(member);
	return table;
}

void CommonObject::RegisterMemberChangeCallBack(void *object, const IMember *member, MEMBER_ONCHANGE_CB callBack, const char *debug)
{
	_callPool->RegisterCallBack(member, callBack, debug);
}

void CommonObject::UnRegisterMemberChangeCallBack(void *object, const IMember *member, MEMBER_ONCHANGE_CB callBack)
{
	_callPool->UnRegisterCallBack(member, callBack);
}