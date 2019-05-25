#ifndef __MysqlBuilder_h__
#define __MysqlBuilder_h__
#include "MultiSys.h"
#include "TString.h"
#include "TList.h"

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

const static s32 MYSQL_TABLE_NAME_LEN = 64;
const static char MYSQL_ESCAPE_CHAR = '`';
const static char MYSQL_STR_SPLIT = '\'';
const static char MYSQL_END_CHAR = ';';

const static char *MYSQL_SPILT_STR = ", ";
const static s32 MYSQL_ESCAPE_STR_ADD = 1;
const static char *MYSQL_KEYWORD_SELECT = "SELECT ";
const static char *MYSQL_KEYWORD_FROM = " FROM ";
const static char *MYSQL_KEYWORD_WHERE = " WHERE ";
typedef s32 (*ESCAPE_STR_FUN)(const void *context, s32 len, const char* pszSrc, int nSize, char* pszDest);
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
		s32 escapeLen = EscapeStr(val, len, dst);
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


class Query : public SQLCommand
{
public:
	Query(void *context, s32 size, ESCAPE_STR_FUN fun, const char *table) : SQLCommand(context, size, fun, table){}

	virtual const char * ToStr();
	Query & Select(const char *name){
		if (!_selects.empty())
			_selects.append(MYSQL_SPILT_STR);
		AddFiledName(_selects, name);
		return *this;
	}

protected:
	NameSet	_selects;
	FinalSQL _final;
};

#endif
