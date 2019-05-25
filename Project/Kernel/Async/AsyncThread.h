#ifndef __ASYNCTHREAD_H__
#define __ASYNCTHREAD_H__
#include "IKernel.h"
#include "../Kernel.h"
#include <thread>

using namespace core;

#define DEBUG_SIZE 256

class AsyncBase : public ITrace {
public:
	AsyncBase(IAsyncHandler * handler, const char * file, s32 line) : _handler(handler) {
		SafeSprintf(_debug, sizeof(_debug), "%d:%s", line, file);
		handler->SetBase(this);

		_valid = true;
		_executed = false;
		_successed = false;
		next = nullptr;
	}
	virtual ~AsyncBase() {}
	virtual const char * GetTraceInfo() { return _debug; };
	inline void SetInvalid() { _valid = false; }

	void OnExecute(s32 queueId, s32 threadIdx) {
		if (_valid) {
			_successed = _handler->OnExecute(Kernel::GetInstancePtr(), queueId, threadIdx);
			_executed = true;
		}
	}

	void OnComplete() {
		if (_successed)
			_handler->OnSuccess(Kernel::GetInstancePtr());
		else
			_handler->OnFailed(Kernel::GetInstancePtr(), _executed);

		_handler->OnRelease(Kernel::GetInstancePtr());
	};

	void Release() { DEL this; };

public:
	AsyncBase *next;

private:
	char _debug[DEBUG_SIZE];
	IAsyncHandler * _handler;

	bool _valid;
	bool _executed;
	bool _successed;
};

class AsyncThread {
	struct AsyncBaseLinkChain {
		AsyncBase * head;
		AsyncBase * tail;
	};

	struct AsyncBaseThreadData 
	{
		AsyncBaseLinkChain main;
		AsyncBaseLinkChain swap;
		AsyncBaseLinkChain work;
		std::mutex lock;
	};

public:
	AsyncThread(s32 ququeId, s32 threadIdx);
	~AsyncThread() {}

	void Start();
	void Terminate();
	void Add(AsyncBase * base);
	void Loop(s64 overtime);

private:
	void ThreadProc();

private:
	AsyncThread(const AsyncThread&) = delete;

private:
	bool _terminate;
	s32  _queueId;
	s32  _threadIdx;
	std::thread _thread;
	AsyncBaseThreadData _readyExec;
	AsyncBaseThreadData _complete;
};

#endif //__ASYNCTHREAD_H__
