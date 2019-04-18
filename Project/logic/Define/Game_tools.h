#ifndef __GameTools_h__
#define __GameTools_h__
#include "MultiSys.h"
#include "IKernel.h"
#include <functional>

typedef std::function<void(core::IKernel *kernel, s64 tick)> StartCallFun;
typedef std::function<void(core::IKernel *kernel, s64 tick)> OnTimeCallFun;
typedef std::function<void(core::IKernel *kernel, s64 tick)> TerminateCallFun;
class SimpleTimer : public core::ITimer
{
public:
	SimpleTimer(const StartCallFun &startFun, const OnTimeCallFun &ontimeFun, const TerminateCallFun &terminateFun):
		_startCallFun(startFun), _onTimeCallFun(ontimeFun), _onTerminateCallFun(terminateFun) {}

		virtual void OnStart(core::IKernel *kernel, s64 tick) {	if (_startCallFun) _startCallFun(kernel, tick);	};
		virtual void OnTime(core::IKernel *kernel, s64 tick) { if (_onTimeCallFun) _onTimeCallFun(kernel, tick); };
		virtual void OnTerminate(core::IKernel *kernel, s64 tick) { if (_onTerminateCallFun) _onTerminateCallFun(kernel, tick); };
protected:
private:
	StartCallFun _startCallFun;
	OnTimeCallFun _onTimeCallFun;
	TerminateCallFun _onTerminateCallFun;
};
#endif
