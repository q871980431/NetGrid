#include "MysqlTest.h"
#include "UnitTest.h"
#include "Game_tools.h"
#include "MysqlBuilder.h"

UnitTestRegister<MysqlTest> mysqlTest;
void MysqlTest::StartTest(core::IKernel *kernel)
{
	core::IKernel *s_kernel = kernel;
	FIND_MODULE(_mysqlMgr, MysqlMgr);
	MainThreadTest();
	MainThreadSQLBuilder(kernel);
}

void MysqlTest::MainThreadTest()
{
	for (s32 i = 0; i < 10; i++)
	{
		MainThreadTestHandler *mysqlHandler = NEW MainThreadTestHandler();
		_mysqlMgr->PushMysqlHandler(i, mysqlHandler);
		MainThreadErrorHandler *testHandler = NEW MainThreadErrorHandler(i);
		_mysqlMgr->PushMysqlHandler(i, testHandler);
	}
}

void MysqlTest::MainThreadSQLBuilder(core::IKernel *kernel)
{
	IEscapeStringHandler *hander = _mysqlMgr->GetEscapeStringHandlerInMainThread();
	if (hander)
	{
		MYSQL_QUERY(query, hander, "Account");
		query.Select("accountid").Select("platform", "username").Where("username", SQLCommand::EQ, "dfgew").And("username", SQLCommand::EQ, "dfgew");
		query.OrderBy("username", false, "platform", "accountid", true).Limit(10);
		const char *sql = query.ToStr();
		TRACE_LOG("SQL:%s", sql);
	}
}

void MainThreadTestHandler::OnSuccess(core::IKernel *kernel, IDBResult *dbResult)
{
	s32 count = dbResult->RowCount();
	for (s32 i = 0; i < count; i++)
	{
		IRowData *row = dbResult->GetRow(i);
		for (s32 j = 0; j < row->FiledCount(); j++)
		{
			const char *content = row->GetString(j);
			TRACE_LOG("Filed[%d][%d], content:%s", i, j, content);
		}
	}

}

void MainThreadTestHandler::OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo)
{
	TRACE_LOG("Exec Failed, ErrorCode:%d, Info:%s", errorCode, errorInfo);
}

void MainThreadErrorHandler::OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo)
{
	TRACE_LOG("Exec Failed, Id:%d, ErrorCode:%d, Info:%s", _id, errorCode, errorInfo);
}