#include "TableObject.h"
void TableObject::DelRow(s32 index)
{
	if (index < (s32)_members.size())
		InnerDelRow(index);
}

void TableObject::SwapRow(s32 index, s32 targetIndex)
{
	s32 size = _members.size();
	ASSERT(index < size, "error");
	ASSERT(targetIndex < size, "error");
	ASSERT(index != targetIndex, "error");
	if (index < size && targetIndex < size)
	{
		std::swap(_members[index], _members[targetIndex]);
		_members[index]->SetIndex(targetIndex);
		_members[targetIndex]->SetIndex(index);
	}
}

RowObject * TableObject::TableAddRow()
{
	RowObject *obj = NEW RowObject(_des);
	obj->SetIndex(_members.size());
	_members.push_back(obj);
	return obj;
}

RowObject * TableObject::GetRow(s32 index) const
{
	ASSERT(index < (s32)_members.size(), "error");
	if (index < (s32)_members.size())
	{
		ASSERT(_members[index]->GetIndex() == index, "error");
		return _members[index];
	}

	return nullptr;
}

void TableObject::InnerDelRow(s32 index)
{
	s32 size = _members.size();
	ASSERT(index < size, "error");
	if (index < size)
	{
		for (s32 i = index; i < size - 1; i++)
		{
			_members[i] = _members[i + 1];
			_members[i]->SetIndex(i);
		}
		_members.pop_back();
	}
}

TableObectInt64Key::TableObectInt64Key(const TableDes *des, S64_COMPARE_FUN fun) :TableObject(des), _index(fun)
{
}

void TableObectInt64Key::DelRow(s32 index)
{
	if (index < (s32)_members.size())
	{
		s64 key = GetRowKey(_members[index]);
		_index.erase(key);
		TableObject::DelRow(index);
	}
}

IRow * TableObectInt64Key::AddRowByKey(s64 key)
{
	auto iter = _index.find(key);
	if (iter == _index.end())
	{
		RowObject *ret = TableObject::TableAddRow();
		SetRowKey(ret, key);
		_index.emplace(key, ret);
		return ret;
	}

	return nullptr;
}

s64 TableObectInt64Key::GetRowKey(IRow *row)
{
	const MemberProperty *_keyMember = _des->GetKeyMemProperty();
	switch (_keyMember->type)
	{
	case DATA_TYPE::DATA_TYPE_BOOL: return row->GetMemberS32(_keyMember);
	case DATA_TYPE::DATA_TYPE_S8: return row->GetMemberS32(_keyMember);
	case DATA_TYPE::DATA_TYPE_S16: return row->GetMemberS32(_keyMember);
	case DATA_TYPE::DATA_TYPE_S32: return row->GetMemberS32(_keyMember);
	case DATA_TYPE::DATA_TYPE_S64: return row->GetMemberS32(_keyMember);
	case DATA_TYPE::DATA_TYPE_FLOAT: return row->GetMemberS32(_keyMember);
	}

	return 0;
}

void TableObectInt64Key::SetRowKey(RowObject *row, s64 key)
{
	const MemberProperty *_keyMember = _des->GetKeyMemProperty();
	switch (_keyMember->type)
	{
	case DATA_TYPE::DATA_TYPE_BOOL: {row->SetKeyAttrT<bool>(_keyMember, (bool)key); return; };
	case DATA_TYPE::DATA_TYPE_S8: {row->SetKeyAttrT<s8>(_keyMember, (s8)key); return; };
	case DATA_TYPE::DATA_TYPE_S16: {row->SetKeyAttrT<s16>(_keyMember, (s16)key); return; };
	case DATA_TYPE::DATA_TYPE_S32: {row->SetKeyAttrT<s32>(_keyMember, (s32)key); return; };
	case DATA_TYPE::DATA_TYPE_S64: {row->SetKeyAttrT<s64>(_keyMember, (s64)key); return; };
	case DATA_TYPE::DATA_TYPE_FLOAT: {row->SetKeyAttrT<float>(_keyMember, (float)key); return; };
	}

	return;
}

void TableObectStrKey::DelRow(s32 index)
{
	if (index < (s32)_members.size())
	{
		const char * key = GetRowKey(_members[index]);
		_index.erase(key);
		TableObject::DelRow(index);
	}
}

IRow * TableObectStrKey::AddRowByKey(const char * key)
{
	auto iter = _index.find(key);
	if (iter == _index.end())
	{
		RowObject *ret = TableObject::TableAddRow();
		SetRowKey(ret, key);
		_index.emplace(key, ret);
		return ret;
	}

	return nullptr;
}

const char * TableObectStrKey::GetRowKey(IRow *row)
{
	const MemberProperty *keyMember = _des->GetKeyMemProperty();
	if (keyMember->type == DATA_TYPE::DATA_TYPE_STR)
		return row->GetMemberStr(keyMember);
	return "";
}

void TableObectStrKey::SetRowKey(RowObject *row, const char * key)
{
	const MemberProperty *keyMember = _des->GetKeyMemProperty();
	row->SetKeyAttrT<const char *>(keyMember, key);
}