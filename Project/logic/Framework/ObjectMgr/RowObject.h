#ifndef __Row_Object_h__
#define __Row_Object_h__
#include "IObjectMgr.h"
#include "ObjectDes.h"
#include "PODObject.h"

class RowObject : public IRow
{
public:
	RowObject(const TableDes *des):_des(des) {
		Init();
	};

	virtual ~RowObject() {
		_des->DestoryMemeory(_memeroy.addr);
		_memeroy.addr = nullptr;
		_memeroy.size = 0;
	};

	virtual s32 GetMemberS32(const IMember *member) { return GetAttrT<s32>((const MemberProperty *)member); };
	virtual s64 GetMemberS64(const IMember *member) { return GetAttrT<s64>((const MemberProperty *)member); };
	virtual const char * GetMemberStr(const IMember *member) { return GetAttrT<const char *>(member); };

	virtual void SetMemberS32(const IMember *member, s32 val) { SetAttrT<s32>((const MemberProperty *)member, val); };
	virtual void SetMemberS64(const IMember *member, s64 val) { SetAttrT<s64>((const MemberProperty *)member, val); };
	virtual void SetMemberStr(const IMember *member, const char *val) { SetAttrT<const char *>((const MemberProperty *)member, val); };

	virtual s32  GetIndex() { return _index; };
	inline void SetIndex(s32 index) { _index = index; };
	template< typename T>
	inline void SetKeyAttrT(const IMember *member, T val)
	{
		_des->SetAttrT<T>(_memeroy.addr, (const MemberProperty *)member, val);
	}
protected:
	bool Init();

private:
	template< typename T>
	inline T GetAttrT(const IMember *member)
	{
		return _des->GetAttrT<T>(_memeroy.addr, (const MemberProperty *)member);
	}

	template< typename T>
	inline void SetAttrT(const MemberProperty *member, T val)
	{
		const MemberProperty *keyMemberProperty = _des->GetKeyMemProperty();
		if (keyMemberProperty == member)
		{
			ASSERT(false, "key not edit");
			return;
		}
		_des->SetAttrT<T>(_memeroy.addr, (const MemberProperty *)member, val);
		//_callPool->Call(member, this, member, &val, sizeof(T));
		//_callPool->Call(nullptr, this, member, &val, sizeof(T));
	}
private:
	const TableDes		*_des;
	Memeroy				_memeroy;
	s32					_index;
};
#endif
