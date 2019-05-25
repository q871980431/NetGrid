/*
 * DBConnection.cc
 *
 *  Created on: Mar 18, 2012
 *      Author: liuqian
 */
#include "DBConnection.h"
#include "Tools_time.h"
#include "Tools.h"
#include "StopWatch.h"
#include <string.h>


DBConnection::DBConnection(core::IKernel *kernel, const char* hostIp, int port, const char* databaseName, const char* username, const char* password, unsigned int connectionflag, const char * charset)
{
	mysql_init( &_mysqlHandler );

	_kernel = kernel;
	_connectionInfo.username = username;
	_connectionInfo.password  = password;
	_connectionInfo.databaseName = databaseName;
	_connectionInfo.port   = port;
	_connectionInfo.clientFlag = connectionflag;
	_connectionInfo.hostDes = hostIp;
	_connectionInfo.charset = charset;
	tools::Zero(_traceInfo);
}

DBConnection::~DBConnection()
{
	mysql_close( &_mysqlHandler );
}

bool DBConnection::open()
{
	core::IKernel *kernel = _kernel;
	int value = 1;
	mysql_options(&_mysqlHandler, MYSQL_OPT_RECONNECT, (char *)&value);

	if(  mysql_real_connect( &_mysqlHandler,
		_connectionInfo.Host(),
		_connectionInfo.User(),
		_connectionInfo.Password(),
		_connectionInfo.DBName(),
		_connectionInfo.Port(),
		_connectionInfo.UnixHanderName(),
		_connectionInfo.ClientFlag() ) == NULL )
	{
		IMPORTANT_LOG(MYSQL_ERROR_LABL, "Failed to connect to database: Error: %s\n", mysql_error(&_mysqlHandler));
		return false;
	}

	if (!_connectionInfo.charset.empty())
	{
		if (mysql_set_character_set(&_mysqlHandler, _connectionInfo.Charset()))
		{
			IMPORTANT_LOG(MYSQL_ERROR_LABL, "Failed to set names to %s: Error: %s\n",
				_connectionInfo.charset.c_str(), mysql_error(&_mysqlHandler));
			return false;
		}
	}

	return true;
}

bool DBConnection::reopen()
{
	mysql_close( &_mysqlHandler );
	mysql_init( &_mysqlHandler );

	return open();
}

bool DBConnection::execute(const char* sql)
{
	_traceInfo._execCount++;
	StopWatch<> watch;
	bool ret = InnerExecute(sql);
	s64 execTime = watch.Interval();
	if (execTime > _traceInfo._maxExecTime)
		_traceInfo._maxExecTime = execTime;
	if (execTime < _traceInfo._minExecTime)
		_traceInfo._minExecTime = execTime;
	if (!ret)
		_traceInfo._failCount++;

	return ret;
}

bool DBConnection::isActive()
{
	if (mysql_ping(&_mysqlHandler) != 0)
	{
		s64 start = tools::GetTimeMillisecond();
		s32 i = 1;
		while (true)
		{
			if (mysql_ping(&_mysqlHandler) == 0)
				return true;
			core::IKernel *kernel = _kernel;
			IMPORTANT_LOG(MYSQL_ERROR_LABL, "Ping Mysql failed, try ping count:%d", i++);
			if (tools::GetTimeMillisecond() - start > (tools::MINUTE * RECONNECT_TIME_OUT))
				break;
		}

		return reopen();
	}

	return true;
}

bool DBConnection::InnerExecute(const char *sql)
{
	int ret = mysql_real_query(&_mysqlHandler, sql, strlen(sql));
	if (ret == 0)
		return true;
	if (!isActive())
		return false;
	ret = mysql_real_query(&_mysqlHandler, sql, strlen(sql));
	return ret == 0 ? true : false;
}

unsigned int DBConnection::GetLastErrNo()
{
	return mysql_errno(&_mysqlHandler);
}

const char* DBConnection::GetLastErrInfo()
{
	return mysql_error(&_mysqlHandler);
}

unsigned long DBConnection::EscapeStringEx(const char* pszSrc, int nSize, char* pszDest )
{
	unsigned long size = mysql_real_escape_string(&_mysqlHandler, pszDest, pszSrc, nSize);
	if (size == 0)
	{
		if (isActive())
			return mysql_real_escape_string(&_mysqlHandler, pszDest, pszSrc, nSize);
	}

	return size;
}

unsigned long DBConnection::GetAffectedRow()
{
	return (unsigned long)mysql_affected_rows(&_mysqlHandler);
}

