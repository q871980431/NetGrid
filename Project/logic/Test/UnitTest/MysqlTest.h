#ifndef __MysqlTest_h__
#define __MysqlTest_h__
#include "IUnitTest.h"
#include "IMysqlMgr.h"
class MysqlTest : public IUnitTestInstance
{
public:
	virtual void StartTest(core::IKernel *kernel);

protected:

private:
	void MainThreadTest();
	void MainThreadSQLBuilder(core::IKernel *kernel);
private:
	IMysqlMgr *_mysqlMgr;
};

class MainThreadTestHandler : public IMysqlHandler
{
public:
	virtual const char * GetExecSql() { return "select * from account;"; };

public:
	//Work In Main Thread
	virtual void OnSuccess(core::IKernel *kernel, IDBResult *dbResult);
	virtual void OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo);
	virtual void OnRelease() { DEL this; };

protected:
private:
};

class MainThreadErrorHandler : public MainThreadTestHandler
{
public:
	MainThreadErrorHandler(s32 id) :_id(id) {};
	virtual const char * GetExecSql() { return "select * from accoun;"; };
	virtual void OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo);
protected:
private:
	s32 _id;
};
#endif
