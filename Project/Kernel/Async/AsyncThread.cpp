#include "AsyncThread.h"
#include "tools.h"
#include "Tools_time.h"
#include "TList.h"
#include "../Exception/ExceptionMgr.h"
#include <mutex>


AsyncThread::AsyncThread(s32 ququeId, s32 threadIdx) :_terminate(false), _queueId(ququeId),  _threadIdx(threadIdx) {

	tools::Zero(_readyExec.main);
	tools::Zero(_readyExec.swap);
	tools::Zero(_readyExec.work);
	tools::Zero(_complete.main);
	tools::Zero(_complete.swap);
	tools::Zero(_complete.work);
}

void AsyncThread::Start() {
	_thread = std::thread(&AsyncThread::ThreadProc, this);
}

void AsyncThread::Terminate() {
	_terminate = true;
	_thread.join();
}

void AsyncThread::Add(AsyncBase * base) {
	tlib::linear::PushTail(_readyExec.main, base);
}

void AsyncThread::Loop(s64 overtime) {
	tlib::linear::MergeListByLock(_readyExec.swap, _readyExec.main, _readyExec.lock);
	tlib::linear::MergeListByLock(_complete.main, _complete.swap, _complete.lock);
	s64 tick = tools::GetTimeMillisecond();
	AsyncBase * base = tlib::linear::PopHead<AsyncBaseLinkChain, AsyncBase>(_complete.main);
	while (base)
	{
		TRY_BEGIN
		base->OnComplete();
		TRY_END
		TRY_BEGIN
		base->Release();
		TRY_END
		if (tools::GetTimeMillisecond() - tick >= overtime)
			break;
		base = tlib::linear::PopHead<AsyncBaseLinkChain, AsyncBase>(_complete.main);
	}
}

void AsyncThread::ThreadProc() {
	while (!_terminate) {
		AsyncBase * base = tlib::linear::PopHead<AsyncBaseLinkChain, AsyncBase>(_readyExec.work);
		if (!base) {
			tlib::linear::MergeListByLock(_readyExec.work, _readyExec.swap, _readyExec.lock);
			base = tlib::linear::PopHead<AsyncBaseLinkChain, AsyncBase>(_readyExec.work);
		}

		if (base) {
			TRY_BEGIN
			base->OnExecute(_queueId, _threadIdx);
			TRY_END
			tlib::linear::PushTail(_complete.work, base);
			if (_complete.swap.head == nullptr)
				tlib::linear::MergeListByLock(_complete.swap, _complete.work, _complete.lock);
		}
		else {
			MSLEEP(1);
		}
	}
}
