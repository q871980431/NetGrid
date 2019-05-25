#include "MysqlBuilder.h"
void SQLCommand::AddSymbol(std::string &src, SYMBOL symbol)
{
	switch (symbol)
	{
	case EQ: src.append(" = "); break;
	case LESS: src.append(" < "); break;
	//case EQ: src.append(" == "); break;
	}
}

const char * Query::ToStr()
{
	if (_final.empty())
	{
		_final.append(MYSQL_KEYWORD_SELECT);
		_final.append(_selects);
		_final.append(MYSQL_KEYWORD_FROM);
		_final.append(_table.GetString());
		_final.append(MYSQL_KEYWORD_WHERE);
		_final.append(_whereConditions);
		_final.push_back(MYSQL_END_CHAR);
	}
	return _final.c_str();
}