#ifndef __Table_Object_h__
#define __Table_Object_h__
#include "IObjectMgr.h"
#include "ObjectDes.h"
#include "PODObject.h"
#include "RowObject.h"
#include <map>

class TableObject : public ITable
{
public:
	TableObject(const TableDes *des) :_des(des) {};
	virtual ~TableObject() {
		for (auto iter : _members)
			DEL iter;
		_members.clear();
	};

	virtual IRow * CreateRow() { return TableAddRow(); };
	virtual IRow * GetRow(s32 index) { return GetRow(index); };
	virtual s32	   RowCount() { return _members.size(); };
	virtual void   DelRow(s32 index);
	virtual void   SwapRow(s32 index, s32 targetIndex);
	virtual void   ForEach(const CallBackFun &fun) {
		if (fun == nullptr)
			return;
		for (auto iter : _members) fun(iter);
	};

public:
	virtual IRow * AddRowByKey(s64 key) { return nullptr; };
	virtual IRow * AddRowByKey(const char *key) { return nullptr; };
	virtual IRow * FindRowByKey(s64 key) { return nullptr; };
	virtual IRow * FindRowByKey(const char *key) { return nullptr; };

public:
	RowObject * TableAddRow();
	RowObject * GetRow(s32 index)const;
	void InnerDelRow(s32 index);

protected:
	const TableDes							*_des;
	std::vector<RowObject*>					_members;
};

typedef bool (*S64_COMPARE_FUN)(const s64 &lVal, const s64 &rVal);

class TableObectInt64Key : public TableObject
{
public:
	TableObectInt64Key(const TableDes *des, S64_COMPARE_FUN fun);
	virtual ~TableObectInt64Key() {};

	virtual IRow * CreateRow() { ASSERT(false, "error"); return nullptr; };
	virtual void DelRow(s32 index);
	virtual void ForEach(const CallBackFun &fun){
		if (fun == nullptr)
			return;
		for (auto &iter : _index) fun(iter.second);
	}
	virtual IRow * AddRowByKey(s64 key);
	virtual IRow * FindRowByKey(s64 key) { return GetRowByKey(key); };

protected:
	inline IRow * GetRowByKey(s64 key) {
		auto iter = _index.find(key);
		return iter != _index.end() ? iter->second : nullptr;
	}
	s64 GetRowKey(IRow *row);
	void SetRowKey(RowObject *row, s64 key);
private:
	std::map<s64, IRow *, S64_COMPARE_FUN>	_index;
};

typedef bool(*STR_COMPARE_FUN)(const std::string &lVal, const std::string &rVal);
class TableObectStrKey : public TableObject
{
public:
	TableObectStrKey(const TableDes *des, STR_COMPARE_FUN fun) :TableObject(des),_index(fun){};
	virtual ~TableObectStrKey() {};

	virtual IRow * CreateRow() { ASSERT(false, "error"); return nullptr; };
	virtual void   DelRow(s32 index);
	virtual void ForEach(const CallBackFun &fun){	
		if (fun == nullptr)
			return;
		for (auto &iter : _index) fun(iter.second);
	}
	virtual IRow * AddRowByKey(const char * key);
	virtual IRow * FindRowByKey(const char * key) { return GetRowByKey(key); };

protected:
	inline IRow * GetRowByKey(const char * key) {
		auto iter = _index.find(key);
		return iter != _index.end() ? iter->second : nullptr;
	}
	const char * GetRowKey(IRow *row);
	void SetRowKey(RowObject *row, const char * key);
private:
	std::map<std::string, IRow *, STR_COMPARE_FUN>	_index;
};
#endif
