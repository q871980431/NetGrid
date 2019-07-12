/*
 * File:	MysqlMgr.h
 * Author:	XuPing
 * 
 * Created On 2019/5/20 16:09:38
 */

#ifndef __MysqlMgr_h__
#define __MysqlMgr_h__
#include "IMysqlMgr.h"
#include <set>
#include <unordered_map>
class DBConnection;

typedef std::unordered_map<std::string, std::vector<DBConnection*>> ConnectionMap;
class MysqlMgr : public IMysqlMgr, public core::ITimer
{
public:
    virtual ~MysqlMgr(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

	virtual void OnStart(IKernel *kernel, s64 tick) {};
	virtual void OnTime(IKernel *kernel, s64 tick);
	virtual void OnTerminate(IKernel *kernel, s64 tick, bool isKill) {};

public:
	virtual bool PushMysqlHandler(const s64 id, IMysqlHandler *handler, const char *connectionName = nullptr);
	virtual s32  EscapeStringExInMainThread(const char* pszSrc, int nSize, char* pszDest, const char *connectionName = nullptr);
	virtual IEscapeStringHandler * GetEscapeStringHandlerInMainThread(const char *connectionName = nullptr);
	virtual s32  EscapeStringExInAsyncThread(const s32 threadIdx, const char* pszSrc, int nSize, char* pszDest, const char *connectionName = nullptr);
	virtual IEscapeStringHandler * GetEscapeStringHandlerInAsyncThread(const s32 threadIdx, const char *connectionName = nullptr);
	virtual bool ExecSqlInAsyncThread(const s32 threadIdx, const char* sql, const char *connectionName = nullptr);
	virtual bool ExecSqlInAsyncThread(const s32 threadIdx, const char* sql, const ExecCallBackFun &cbFun, const char *connectionName = nullptr);

protected:
	bool LoadConfigFile(IKernel *kernel);
	DBConnection * GetDBConnection(const u32 threadIdx, const char *connectionName);
	inline u32 GetConnectionThreadIdx(u64 id) { return id % _asyncConnectionNum; };
	inline u32 GetConnectionMainThreadIdx() { return _asyncConnectionNum; };
	inline bool CheckThreadIdx(s32 threadIdx) { return 0 <= threadIdx && (threadIdx < _asyncConnectionNum); };

private:
    static MysqlMgr     * s_self;
    static IKernel		* s_kernel;

	std::set<s32>			_threadIds;
	ConnectionMap			_connctionMap;
	s32						_asyncConnectionNum;
	std::vector<DBConnection*> *_mainConnection;
	s32						_perfileTime;
};
#endif