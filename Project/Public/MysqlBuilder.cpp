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
		_final.append(_orderBy);
		if (_rowCount != 0)
		{
			_final.append(MYSQL_KEYWORD_LIMIT);
			AddFiledVal(_final, _offSet);
			_final.append(MYSQL_SPILT_STR);
			AddFiledVal(_final, _rowCount);
		}
		_final.push_back(MYSQL_END_CHAR);
	}
	return _final.c_str();
}