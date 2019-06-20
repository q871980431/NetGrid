#ifndef __Folly_Test_h__
#define __Folly_Test_h__
#include "IUnitTest.h"
#include "AtomicIntrusiveLinkedList.h"

struct PlayerData 
{
	s32 id;
	folly::AtomicIntrusiveLinkedListHook<PlayerData> hook;
};

typedef folly::AtomicIntrusiveLinkedList<PlayerData, &PlayerData::hook> AtomicList;
class FollyTest : public IUnitTestInstance
{
public:

	virtual void StartTest(core::IKernel *kernel);
protected:
	void TestAtomicList(core::IKernel *kernel);

private:
	AtomicList _atomicList;
};
#endif
