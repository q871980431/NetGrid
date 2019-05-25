#include "AsyncQueue.h"
#include "XmlReader.h"
#include "../kernel.h"
#include "ConfigMgr.h"
#include "AsyncThread.h"

bool AsyncQueue::Ready() {
    return true;
}

bool AsyncQueue::Initialize(s32 threadCount)
{
	if (threadCount > 0) {
		for (s32 i = 0; i < threadCount; ++i) {
			AsyncThread * t = NEW AsyncThread(_queueId, i);
			t->Start();
			_threads.push_back(t);
		}
	}

    return true;
}

bool AsyncQueue::Destroy() {
	for (AsyncThread * t : _threads) {
		t->Terminate();
		DEL t;
	}
	_threads.clear();

    DEL this;
    return true;
}

void AsyncQueue::Loop(s64 overTime) {
	for (AsyncThread * t : _threads)
		t->Loop(overTime / _threads.size());
}

void AsyncQueue::StartAsync(const s64 threadId, IAsyncHandler * handler, const char * file, const s32 line) {
	ASSERT(!handler->GetBase(), "wtf");
	ASSERT(_threads.size() > 0, "wtf");
	AsyncBase * base = NEW AsyncBase(handler, file, line);
	_threads[((u64)threadId) % _threads.size()]->Add(base);
}

void AsyncQueue::StopAsync(IAsyncHandler * handler) {
	ASSERT(handler->GetBase(), "wtf");
	((AsyncBase*)handler->GetBase())->SetInvalid();
}

void AsyncQueue::GetQueueInfo(s32 &threadNum, std::set<s32> &threadIds)
{
	threadNum = _threads.size();
	for (s32 i = 0; i < threadNum; i++)
		threadIds.insert(i);
}