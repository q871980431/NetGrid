#ifndef __MysqlBuilder_h__
#define __MysqlBuilder_h__
#include "MultiSys.h"
#include "TString.h"
#include "TList.h"
#include "Tools.h"

class FiledName
{
	std::string _name;
	FiledName *next;
	FiledName(const char *name) :_name(name), next(nullptr) {};
};

class ConditionExpres
{
public:
protected:
private:
};

//typedef tlib::linear::SimpleList<FiledName> NameSet;
typedef std::string NameSet;
typedef std::string ConditionSet;
typedef std::string FinalSQL;
typedef std::string OrderByExp;
typedef std::string SetFiledSQL;

const static s32 MYSQL_TABLE_NAME_LEN = 64;
const static char MYSQL_ESCAPE_CHAR = '`';
const static char MYSQL_STR_SPLIT = '\'';
const static char MYSQL_END_CHAR = ';';

const static char *MYSQL_SPILT_STR = ", ";
const static s32 MYSQL_ESCAPE_STR_ADD = 1;
const static char *MYSQL_KEYWORD_SELECT = "SELECT ";
const static char *MYSQL_KEYWORD_FROM = " FROM ";
const static char *MYSQL_KEYWORD_WHERE = " WHERE ";
const static char *MYSQL_KEYWORD_ORDERBY = " ORDER BY ";
const static char *MYSQL_KEYWORD_DESC = " DESC ";
const static char *MYSQL_KEYWORD_ASC = " ASC ";
const static char *MYSQL_KEYWORD_LIMIT = " LIMIT ";


typedef s32(*ESCAPE_STR_FUN)(const void *context, s32 len, const char* pszSrc, int nSize, char* pszDest);

#define WHERE_LOGIC_EXP_DEC(H, name)\
	template< class T>\
SQLCommand & H##name(const char *filedName, SYMBOL symbol, T val)\

#define WHERE_LOGIC_EXP_DEF(H, name)\
template< class T>\
inline SQLCommand & SQLCommand::H##name(const char *filedName, SYMBOL symbol, T val)\
{\
	if (!_whereConditions.empty())\
	{\
		_whereConditions.append(" "#H#name" ");\
		AddFiledName(_whereConditions, filedName);\
		AddSymbol(_whereConditions, symbol);\
		AddFiledVal(_whereConditions, val);\
	}\
	return *this;\
}\
template<>\
inline SQLCommand & SQLCommand::H##name<const char *>(const char *filedName, SYMBOL symbol, const char *val)\
{\
	if (!_whereConditions.empty())\
	{\
		_whereConditions.append(" "#H#name" "); \
		AddFiledName(_whereConditions, filedName);\
		AddSymbol(_whereConditions, symbol);\
		AddFiledVal(_whereConditions, val, strlen(val));\
	}\
	return *this;\
}


class SQLCommand
{
public:
	enum SYMBOL
	{
		EQ = 1,
		LESS = 2,
	};
	SQLCommand(void *context, s32 size, ESCAPE_STR_FUN fun, const char *table) : _size(size), _fun(fun) {
		_context = malloc(size);
		memcpy(_context, context, size);
		_table = MYSQL_ESCAPE_CHAR;
		_table << table;
		_table << MYSQL_ESCAPE_CHAR;
	}
	virtual const char * ToStr() = 0;
	template< class T>
	SQLCommand & Where(const char *filedName, SYMBOL symbol, T val);
	WHERE_LOGIC_EXP_DEC(A,nd);
	WHERE_LOGIC_EXP_DEC(O,r);
protected:
	void AddFiledName(std::string &src, const char *filedName)
	{
		src.push_back(MYSQL_ESCAPE_CHAR);
		src.append(filedName);
		src.push_back(MYSQL_ESCAPE_CHAR);
	}

	void AddSymbol(std::string &src, SYMBOL symbol);
	template<typename T>
	void AddFiledVal(std::string &src, const T val) {
		char buff[32];
		tools::ValToStr(buff, sizeof(buff), val);
		src.append(buff);
	}

	void AddFiledVal(std::string &src, const char *val, s32 len)
	{
		char *dst = (char *)alloca(GetEscapeBuffSize(len));
		EscapeStr(val, len, dst);
		src.push_back(MYSQL_STR_SPLIT);
		src.append(dst);
		src.push_back(MYSQL_STR_SPLIT);
	}

	s32 EscapeStr(const char* pszSrc, int nSize, char* pszDest){return _fun(_context, _size, pszSrc, nSize, pszDest);}
	s32 GetEscapeBuffSize(s32 nSize) { return 2 * nSize + MYSQL_ESCAPE_STR_ADD; };

protected:
	void *_context;
	s32 _size;
	ESCAPE_STR_FUN _fun;

	tlib::TString<MYSQL_TABLE_NAME_LEN> _table;
	ConditionSet	_whereConditions;
};

template< class T>
inline SQLCommand & SQLCommand::Where(const char *filedName, SYMBOL symbol, T val)
{
	if (_whereConditions.empty())
	{
		AddFiledName(_whereConditions, filedName);
		AddSymbol(_whereConditions, symbol);
		AddFiledVal(_whereConditions, val);
	}
	return *this;
}
template<>
inline SQLCommand & SQLCommand::Where<const char *>(const char *filedName, SYMBOL symbol, const char *val)
{
	if (_whereConditions.empty())
	{
		AddFiledName(_whereConditions, filedName);
		AddSymbol(_whereConditions, symbol);
		AddFiledVal(_whereConditions, val, strlen(val));
	}
	return *this;
}
WHERE_LOGIC_EXP_DEF(A, nd)
WHERE_LOGIC_EXP_DEF(O, r)

class Query : public SQLCommand
{
public:
	Query(void *context, s32 size, ESCAPE_STR_FUN fun, const char *table) : SQLCommand(context, size, fun, table), _rowCount(0), _offSet(0){}

	virtual const char * ToStr();
	Query & Select(const char *name){
		if (!_selects.empty())
			_selects.append(MYSQL_SPILT_STR);
		AddFiledName(_selects, name);
		return *this;
	}

	template<typename... Args>
	Query & Select(Args... args)
	{
		s32 a[] = { (Select(args),0)... };
		return *this;
	}

	Query & OrderBy(const char *name, bool desc = true)
	{
		if (_orderBy.empty())
			_orderBy.append(MYSQL_KEYWORD_ORDERBY);
		else
			_orderBy.append(MYSQL_SPILT_STR);
		AddFiledName(_orderBy, name);
		if (desc)
			_orderBy.append(MYSQL_KEYWORD_DESC);
		else
			_orderBy.append(MYSQL_KEYWORD_ASC);
		return *this;
	}

	template<typename... Args>
	Query & OrderBy(const char *name, bool second, Args... args)
	{
		OrderBy(name, second);
		OrderBy(args...);
		return *this;
	}

	template<typename... Args>
	Query & OrderBy(const char *name, Args... args)
	{
		OrderBy(name);
		OrderBy(args...);
		return *this;
	}

	Query & Limit(s32 count, s32 offset = 0)
	{
		if (_rowCount == 0)
		{
			_rowCount = count;
			_offSet = offset;
		}
		return *this;
	}

protected:
	NameSet	_selects;
	FinalSQL _final;
	OrderByExp _orderBy;
	s32 _rowCount;
	s32 _offSet;
};


class Update : public SQLCommand
{
public:
	Update(void *context, s32 size, ESCAPE_STR_FUN fun, const char *table) : SQLCommand(context, size, fun, table){}
	virtual const char * ToStr();

	template<typename T>
	struct SetFiled 
	{
		SetFiled(const char *name, T &val)
		{
			_name = name;
			_val = val;
		}
		const char *_name;
		T _val;
	};

	//void Set()
protected:
private:
	SetFiledSQL	_setFiledSQL;
};
#endif
