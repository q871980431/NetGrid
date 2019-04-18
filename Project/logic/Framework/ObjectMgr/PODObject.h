#ifndef __POD_Object_h__
#define __POD_Object_h__
#include "IObjectMgr.h"
#include "ObjectDes.h"

class PODObject : public IPod
{
public:
	PODObject(const ObjectDes *des) :_des(des) {
		_memeroy.addr = _des->CreateMemeory(_memeroy.size);
	};
	virtual ~PODObject() {
		_des->DestoryMemeory(_memeroy.addr);
		_memeroy.addr = nullptr;
		_memeroy.size = 0;
	}

	virtual s32 GetMemberS32(const IMember *member) { return GetAttrT<s32>(member); };
	virtual s64 GetMemberS64(const IMember *member) { return GetAttrT<s64>(member); };
	virtual const char * GetMemberStr(const IMember *member) { return GetAttrT<const char *>(member); };

	virtual void SetMemberS32(const IMember *member, s32 val) { SetAttrT<s32>(member, val); };
	virtual void SetMemberS64(const IMember *member, s64 val) { SetAttrT<s64>(member, val); };;
	virtual void SetMemberStr(const IMember *member, const char *val) { SetAttrT<const char *>(member, val); };

protected:
	template< typename T>
	T GetAttrT(const IMember *member);

	template< typename T>
	inline void SetAttrT(const IMember *member, T val)
	{
		if (((const MemberProperty *)member)->des->memeoryDes != _des)
		{
			ASSERT(false, "error");
			return;
		}
		_des->SetAttrT<T>(_memeroy.addr, (const MemberProperty *)member, val);
	}

protected:
	const ObjectDes	*_des;
	Memeroy				_memeroy;
};

template< typename T>
T PODObject::GetAttrT(const IMember *member)
{
	if (((const MemberProperty *)member)->des->memeoryDes != _des)
	{
		ASSERT(false, "error");
		return T();
	}
	return _des->GetAttrT<T>(_memeroy.addr, (const MemberProperty *)member);
}

#endif
