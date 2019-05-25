#include "MysqlBase.h"
#include "DBConnection.h"

void MysqlResult::Init(MYSQL *mysql)
{
	_result = mysql_store_result(mysql);
	if (_result)
	{
		_fieldCount = mysql_num_fields(_result);
		s32 _rowCount = mysql_num_rows(_result);
		MYSQL_ROW row = nullptr;
		RowFieldLenType *_curRowFieldLengths = nullptr;

		while (row = mysql_fetch_row(_result))
		{
			_curRowFieldLengths = mysql_fetch_lengths(_result);
			if (_curRowFieldLengths == nullptr)
			{
				ASSERT(false, "error");
			}
			_rows.emplace_back(row, _curRowFieldLengths, _fieldCount);
		}
	}
}

bool MysqlBase::OnExecute(IKernel * kernel, s32 queueId, s32 threadIdx)
{
	bool ret = _connection->execute(_mysqlHandler->GetExecSql());
	if (ret)
		_result.Init(_connection->GetMYSQL());
	else
	{
		_erorCode = _connection->GetLastErrNo();
		_errorInfo = _connection->GetLastErrInfo();
	}
	return ret;
}

void MysqlBase::OnSuccess(IKernel * kernel)
{
	_mysqlHandler->OnSuccess(kernel, &_result);
}

void MysqlBase::OnFailed(IKernel * kernel, bool isExecuted)
{
	_mysqlHandler->OnFail(kernel, _erorCode, _errorInfo.c_str());
}

void MysqlBase::OnRelease(IKernel * kernel)
{
	_mysqlHandler->OnRelease();
	DEL this;
}