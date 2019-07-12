#ifndef __MysqlBase_h__
#define __MysqlBase_h__
#include "IMysqlMgr.h"
#include "mysql.h"
#include <vector>
typedef unsigned long RowFieldLenType;
class FiledRow : public IRowData
{
public:
	FiledRow(MYSQL_ROW row, RowFieldLenType *rowFieldLengths, s32 filedCount) :_row(row), _rowFieldLengths(rowFieldLengths), _filedCount(filedCount) {};
	FiledRow(const FiledRow &val) { _rowFieldLengths = val._rowFieldLengths; _row = val._row; _filedCount = val._filedCount; };

	virtual s32 FiledCount() { return _filedCount; }
	virtual const char * GetString(s32 index) {
		if (index < _filedCount)
			return _row[index];
		return nullptr;
	};
	virtual s32 GetBuff(s32 index, char *buff, s32 buffLen) {
		if (index < _filedCount && _row[index])
		{
			s32 dateLen = _rowFieldLengths[index];
			if (dateLen > buffLen)
				dateLen = buffLen;
			if (dateLen > 0)
				memcpy(buff, _row[index], dateLen);
			return dateLen;
		}

		return 0;
	};
private:
	MYSQL_ROW _row;
	RowFieldLenType *_rowFieldLengths;
	s32		_filedCount;
};

class MysqlResult : public IDBResult
{
public:
	MysqlResult() :_result(nullptr){}
	virtual s32 RowCount() { return (s32)_rows.size(); };
	virtual IRowData * GetRow(s32 index) {return index < (s32)_rows.size() ? (&_rows[index]) : nullptr;};

public:
	void Init(MYSQL *mysql);

protected:
private:
	MYSQL_RES* _result;
	s32		   _fieldCount;
	std::vector<FiledRow> _rows;
};
class DBConnection;
class MysqlMgr;
class MysqlBase : public IMysqlBase, public core::IAsyncHandler
{
public:
	MysqlBase(MysqlMgr *mysqlMgr, DBConnection *connection, IMysqlHandler *handler) : _mysqlMgr(mysqlMgr), _connection(connection), _mysqlHandler(handler) {};

	virtual bool OnExecute(IKernel * kernel, s32 queueId, s32 threadIdx);
	virtual void OnSuccess(IKernel * kernel);
	virtual void OnFailed(IKernel * kernel, bool isExecuted);
	virtual void OnRelease(IKernel * kernel);
protected:
private:
	MysqlMgr	  *_mysqlMgr;
	DBConnection  *_connection;
	IMysqlHandler *_mysqlHandler;
	MysqlResult		_result;
	s32				_erorCode;
	std::string		_errorInfo;
};

#endif
