#ifndef __Common_Object_h__
#define __Common_Object_h__
#include "IObjectMgr.h"
#include "ObjectDes.h"
#include "PODObject.h"

class CommonObject : public IObject
{
public:
	CommonObject(const ObjectDes *des):_des(des) {
		Init(0);
	};

	CommonObject(const ObjectDes *des, s64 id) :_des(des) {
		Init(id);
	};


	virtual ~CommonObject() {
		_des->DestoryMemeory(_memeroy.addr);
		_memeroy.addr = nullptr;
		_memeroy.size = 0;
		DEL _callPool;
	};

	virtual s32 GetMemberS32(const IMember *member) { return GetAttrT<s32>(member); };
	virtual s64 GetMemberS64(const IMember *member) { return GetAttrT<s64>(member); };
	virtual const char * GetMemberStr(const IMember *member) { return GetAttrT<const char *>(member); };

	virtual void SetMemberS32(const IMember *member, s32 val) { SetAttrT<s32>(member, val); };
	virtual void SetMemberS64(const IMember *member, s64 val) { SetAttrT<s64>(member, val); };;
	virtual void SetMemberStr(const IMember *member, const char *val) { SetAttrT<const char *>(member, val); };

	virtual ITable  * GetTable(const IMember *member);
public:
	bool Init(s64 GUID);

	void RegisterMemberChangeCallBack(void *object, const IMember *member, MEMBER_ONCHANGE_CB callBack, const char *debug);
	void UnRegisterMemberChangeCallBack(void *object, const IMember *member, MEMBER_ONCHANGE_CB callBack);

private:
	template< typename T>
	inline T GetAttrT(const IMember *member)
	{
		return _des->GetAttrT<T>(_memeroy.addr, (const MemberProperty *)member);
	}

	template< typename T>
	inline void SetAttrT(const IMember *member, T val)
	{
		_des->SetAttrT<T>(_memeroy.addr, (const MemberProperty *)member, val);
		_callPool->Call(member, this, member, &val, sizeof(T));
		_callPool->Call(nullptr, this, member, &val, sizeof(T));
	}
private:
	s64					_GUID;
	MemberChangeCallPool *_callPool;
	const ObjectDes	*_des;
	Memeroy				_memeroy;
};
#endif
