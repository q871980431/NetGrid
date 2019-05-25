#ifndef __GameTools_h__
#define __GameTools_h__
#include "MultiSys.h"
#include "IKernel.h"
#include "IMysqlMgr.h"
#include <functional>

typedef std::function<void(core::IKernel *kernel, s64 tick)> StartCallFun;
typedef std::function<void(core::IKernel *kernel, s64 tick)> OnTimeCallFun;
typedef std::function<void(core::IKernel *kernel, s64 tick, bool isKill)> TerminateCallFun;
class SimpleTimer : public core::ITimer
{
public:
	SimpleTimer(const StartCallFun &startFun, const OnTimeCallFun &ontimeFun, const TerminateCallFun &terminateFun):
		_startCallFun(startFun), _onTimeCallFun(ontimeFun), _onTerminateCallFun(terminateFun) {}

		virtual void OnStart(core::IKernel *kernel, s64 tick) {	if (_startCallFun) _startCallFun(kernel, tick);	};
		virtual void OnTime(core::IKernel *kernel, s64 tick) { if (_onTimeCallFun) _onTimeCallFun(kernel, tick); };
		virtual void OnTerminate(core::IKernel *kernel, s64 tick, bool isKill) { if (_onTerminateCallFun) _onTerminateCallFun(kernel, tick, isKill); };
protected:
private:
	StartCallFun _startCallFun;
	OnTimeCallFun _onTimeCallFun;
	TerminateCallFun _onTerminateCallFun;
};

typedef std::function<bool(core::IKernel *kernel, s32 queueId, s32 threadIdx)> AsyncExecFun;
typedef std::function<void(core::IKernel *kernel)> SuccessFun;
typedef std::function<void(core::IKernel *kernel, bool isExecuted)> OnFailedFun;
class SimpleAsyncTask : public core::IAsyncHandler
{
public:
	SimpleAsyncTask(const AsyncExecFun &asyncFun, const SuccessFun &sucessFun, const OnFailedFun &failedFun) :_asyncExecFun(asyncFun),
		_sucessFun(sucessFun), _failedFun(failedFun) {};

	virtual bool OnExecute(IKernel * kernel, s32 queueId, s32 threadIdx) {
		if (_asyncExecFun) 
			return _asyncExecFun(kernel, queueId, threadIdx);
		return false;
	};
	virtual void OnSuccess(IKernel * kernel) { if (_sucessFun)_sucessFun(kernel); };
	virtual void OnFailed(IKernel * kernel, bool isExecuted) { if (_failedFun)_failedFun(kernel, isExecuted); };
	virtual void OnRelease(IKernel * kernel) { DEL this; };
protected:
private:
	AsyncExecFun _asyncExecFun;
	SuccessFun _sucessFun;
	OnFailedFun _failedFun;
};

inline s32 MysqlEscapeStrFun(const void *context, s32 len, const char* pszSrc, int nSize, char* pszDest)
{
	ASSERT(len == sizeof(IEscapeStringHandler*), "error");
	IEscapeStringHandler *escapeHandler = *(IEscapeStringHandler**)context;
	return escapeHandler->EscapeString(pszSrc, nSize, pszDest);
}

#define MYSQL_QUERY(query, escapeHandler, table)	Query query(&escapeHandler, sizeof(escapeHandler), MysqlEscapeStrFun, table)

#endif
