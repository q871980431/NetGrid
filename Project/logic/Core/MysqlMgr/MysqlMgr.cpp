/*
 * File:	MysqlMgr.cpp
 * Author:	XuPing
 * 
 * Created On 2019/5/20 16:09:38
 */

#include "MysqlMgr.h"
#include "XmlReader.h"
#include "DBConnection.h"
#include "Tools_time.h"
#include "MysqlBase.h"

MysqlMgr * MysqlMgr::s_self = nullptr;
IKernel * MysqlMgr::s_kernel = nullptr;
bool MysqlMgr::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	if (!LoadConfigFile(kernel))
		return false;

    return true;
}

bool MysqlMgr::Launched(IKernel *kernel)
{
	if (_perfileTime == 0)
		_perfileTime = tools::MINUTE;
	ADD_TIMER(this, _perfileTime, FOREVER, _perfileTime);
    return true;
}

bool MysqlMgr::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

void MysqlMgr::OnTime(IKernel *kernel, s64 tick)
{
	TRACE_LOG("MysqlProfile:");
	for (const auto &iter : _connctionMap)
	{
		TRACE_LOG("Connection:%s, Info:", iter.first.c_str());
		s32 i = 0;
		for (const auto &connecterIter: iter.second)
		{
			ConnectionTraceInfo *traceInfo = connecterIter->GetTraceInfo();
			TRACE_LOG("Connecter[%d], execute count:%ld, failed count:%ld,  min exec time:%d ms, max exec time:%d ms", i++, traceInfo->_execCount, traceInfo->_failCount, traceInfo->_minExecTime, traceInfo->_maxExecTime)
		}
	}
	TRACE_LOG("MysqlProfile End.");
}

bool MysqlMgr::PushMysqlHandler(const s64 id, IMysqlHandler *handler, const char *connectionName /* = nullptr */)
{
	if (!handler || (handler->GetBase() != nullptr))
	{
		ASSERT(false, "handler error");
		return false;
	}
	u32 threadIdx = GetConnectionThreadIdx(id);
	DBConnection *connection = GetDBConnection(threadIdx, connectionName);
	if (connection == nullptr)
	{
		ASSERT(false, "connection error");
		return false;
	}

	MysqlBase *base = NEW MysqlBase(this, connection, handler);
	handler->SetBase(base);
	s_kernel->StartAsync(threadIdx, base, __FILE__, __LINE__);
	return true;
}

s32 MysqlMgr::EscapeStringExInMainThread(const char* pszSrc, int nSize, char* pszDest, const char *connectionName /* = nullptr */)
{
	DBConnection *connection = GetDBConnection(GetConnectionMainThreadIdx(), connectionName);
	if (connection)
		return connection->EscapeStringEx(pszSrc, nSize, pszDest);
	return 0;
}

IEscapeStringHandler * MysqlMgr::GetEscapeStringHandlerInMainThread(const char *connectionName /* = nullptr */)
{
	return GetDBConnection(GetConnectionMainThreadIdx(), connectionName);
}

s32 MysqlMgr::EscapeStringExInAsyncThread(const s32 threadIdx, const char* pszSrc, int nSize, char* pszDest, const char *connectionName /* = nullptr */)
{
	if (!CheckThreadIdx(threadIdx))
	{
		ASSERT(false, "threadIdx error");
		return 0;
	}
	DBConnection *connection = GetDBConnection(threadIdx, connectionName);
	if (connection)
		return connection->EscapeStringEx(pszSrc, nSize, pszDest);
	return 0;
}

IEscapeStringHandler * MysqlMgr::GetEscapeStringHandlerInAsyncThread(const s32 threadIdx, const char *connectionName /* = nullptr */)
{
	if (!CheckThreadIdx(threadIdx))
	{
		ASSERT(false, "threadIdx error");
		return 0;
	}
	return GetDBConnection(threadIdx, connectionName);
}

bool MysqlMgr::ExecSqlInAsyncThread(const s32 threadIdx, const char* sql, const char *connectionName /* = nullptr */)
{
	if (!CheckThreadIdx(threadIdx))
	{
		ASSERT(false, "threadIdx error");
		return false;
	}
	DBConnection *connection = GetDBConnection(threadIdx, connectionName);
	if (connection)
		return connection->execute(sql);
	return false;
}

bool MysqlMgr::ExecSqlInAsyncThread(const s32 threadIdx, const char* sql, const ExecCallBackFun &cbFun, const char *connectionName /* = nullptr */)
{
	if (!CheckThreadIdx(threadIdx))
	{
		ASSERT(false, "threadIdx error");
		return false;
	}
	DBConnection *connection = GetDBConnection(threadIdx, connectionName);
	if (connection)
	{
		if (connection->execute(sql))
		{
			if (cbFun)
			{
				MysqlResult result;
				result.Init(connection->GetMYSQL());
				cbFun(s_kernel, &result);
			}

			return true;
		}
		else
		{
			s32 errorCode = connection->GetLastErrNo();
			const char *errorInfo = connection->GetLastErrInfo();
			//TO DO
		}
	}

	return false;
}

bool MysqlMgr::LoadConfigFile(IKernel *kernel)
{
	const char *path = kernel->GetConfigFile();
	XmlReader doc;
	if (!doc.LoadFile(path))
	{
		ASSERT(false, "load file error");
		return false;
	}

	IXmlObject *async = doc.Root()->GetFirstChrild("async");
	s32 threadCount = 0;
	if (async)
		threadCount = async->GetAttribute_S32("threadcount");

	IXmlObject *mysql = doc.Root()->GetFirstChrild("mysql");
	if (mysql)
	{
		const char *defaultConnetion = mysql->GetAttribute_Str("defultconnection");
		s32 connectionNum = mysql->GetAttribute_S32("connectionnum");
		_perfileTime = mysql->GetAttribute_S32("perfiletime") * tools::MILLISECONDS;
		if (connectionNum < 0)
			connectionNum = 1;
		if (connectionNum > threadCount)
			connectionNum = threadCount;
		connectionNum++;

		IXmlObject *connection = mysql->GetFirstChrild("connection");
		while (connection)
		{
			const char *name = connection->GetAttribute_Str("name");
			const char *hostIp = connection->GetAttribute_Str("hostip");
			s32 port = connection->GetAttribute_S32("port");
			const char *username = connection->GetAttribute_Str("username");
			const char *password = connection->GetAttribute_Str("password");
			const char *databasename = connection->GetAttribute_Str("databasename");
			const char *characterset = connection->GetAttribute_Str("characterset");

			auto iter = _connctionMap.find(name);
			if (iter != _connctionMap.end())
			{
				ASSERT(false, "mysql config error");
				return false;
			}
			else
			{
				auto ret = _connctionMap.emplace(name, std::vector<DBConnection*>());
				iter = ret.first;
			}

			for (s32 i = 0; i < connectionNum; i++)
			{
				DBConnection *connection = NEW DBConnection(kernel, hostIp, port, databasename, username, password, 0, characterset);
				if (!connection->open())
				{
					IMPORTANT_LOG(MYSQL_ERROR_LABL, "Open connection failed, databasename:%s, hostIp:%s, port:%d, username:%s, password:%s", databasename, hostIp, port, username, password);
					DEL connection;
					return false;
				}
				iter->second.push_back(connection);
			}
			_asyncConnectionNum = connectionNum - 1;
			connection = connection->GetNextSibling();
		}

		auto iter = _connctionMap.find(defaultConnetion);
		if (iter == _connctionMap.end())
		{
			ASSERT(false, "don't find default connection:%s", defaultConnetion);
			return false;
		}
		_mainConnection = &iter->second;
		return true;
	}

	return false;
}

DBConnection * MysqlMgr::GetDBConnection(const u32 threadIdx, const char *connectionName)
{
	if (connectionName == nullptr)
		return (*_mainConnection)[threadIdx];
	else
	{
		auto iter = _connctionMap.find(connectionName);
		if (iter != _connctionMap.end())
			return iter->second[threadIdx];
		else
			return nullptr;
	}
}