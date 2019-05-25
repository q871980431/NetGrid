/*
 * File:	IMysqlMgr.h
 * Author:	XuPing
 * 
 * Created On 2019/5/20 16:09:38
 */

#ifndef  __IMysqlMgr_h__
#define __IMysqlMgr_h__
#include "IModule.h"
#include "Tools.h"
class IMysqlBase
{
public:
	virtual ~IMysqlBase() {};
};

class IRowData
{
public:
	virtual ~IRowData() {};
	
	virtual s32 FiledCount() = 0;
	virtual const char * GetString(s32 index) = 0;
	virtual s32 GetBuff(s32 index, char *buff, s32 buffLen) = 0;

public:
	bool	GetDataBool(s32 index) { return tools::StrToBool(GetString(index)); };
	s8		GetDataS8(s32 index) { return tools::StrToInt8(GetString(index)); };
	s16		GetDataS16(s32 index) { return tools::StrToInt16(GetString(index)); };
	s32		GetDataS32(s32 index) { return tools::StrToInt32(GetString(index)); };
	s64		GetDataS64(s32 index) { return tools::StrToInt64(GetString(index)); };
	float	GetDataFloat(s32 index) { return tools::StrToFloat(GetString(index)); };
};

class IDBResult
{
public:
	virtual ~IDBResult() {};
	virtual s32 RowCount() = 0;
	virtual IRowData * GetRow(s32 index) = 0;
};

class IMysqlHandler
{
public:
	IMysqlHandler() :_base(nullptr) {};
	virtual ~IMysqlHandler() {};

	//Work In Work Thread
	virtual const char * GetExecSql() = 0;
	
public:
	//Work In Main Thread
	virtual void OnSuccess(core::IKernel *kernel, IDBResult *dbResult) = 0;
	virtual void OnFail(core::IKernel *kernel, const s32 errorCode, const char *errorInfo) = 0;
	virtual void OnRelease() = 0;

public:
	inline void SetBase(IMysqlBase *base) { _base = base; };
	inline IMysqlBase * GetBase() { return _base; };

protected:
	IMysqlBase *_base;
};

class IEscapeStringHandler
{
public:
	virtual ~IEscapeStringHandler() {};
	virtual s32   EscapeString(const char* pszSrc, int nSize, char* pszDest) = 0;
};


class IMysqlMgr : public IModule
{
public:
	typedef std::function<void(core::IKernel *kernel, const IDBResult *dbResult)> ExecCallBackFun;
    virtual ~IMysqlMgr(){};
    
	//执行于主线程中的同步接口
	//相同的connectionId执行具有先后顺序 connectionName为null 则为默认的数据库连接名
	virtual bool  PushMysqlHandler(const s64 id, IMysqlHandler *handler, const char *connectionName = nullptr) = 0;
	virtual s32   EscapeStringExInMainThread(const char* pszSrc, int nSize, char* pszDest, const char *connectionName = nullptr) = 0;
	virtual IEscapeStringHandler * GetEscapeStringHandlerInMainThread(const char *connectionName = nullptr) = 0;

public:
	//可执行于异步线程中的同步接口
	virtual s32  EscapeStringExInAsyncThread(const s32 threadIdx, const char* pszSrc, int nSize, char* pszDest, const char *connectionName = nullptr) = 0;
	virtual bool ExecSqlInAsyncThread(const s32 threadIdx, const char* sql, const char *connectionName = nullptr) = 0;
	virtual bool ExecSqlInAsyncThread(const s32 threadIdx, const char* sql, const ExecCallBackFun &cbFun, const char *connectionName = nullptr) = 0;
	virtual IEscapeStringHandler * GetEscapeStringHandlerInAsyncThread(const s32 threadIdx, const char *connectionName = nullptr) = 0;
};


#endif