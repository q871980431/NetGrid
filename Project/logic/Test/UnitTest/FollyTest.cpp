#include "FollyTest.h"
#include "UnitTest.h"
UnitTestRegister<FollyTest> follyTest;
void FollyTest::StartTest(core::IKernel *kernel)
{
	TestAtomicList(kernel);
}

void FollyTest::TestAtomicList(core::IKernel *kernel)
{
	for (s32 i = 0; i < 10; i++)
	{
		PlayerData *date = NEW PlayerData();
		date->id = i;
		_atomicList.insertHead(date);
	}
	auto f = [](PlayerData *data)
	{
		printf("Data Id:%d\n", data->id);
	};
	//_atomicList.sweep(f);
	printf("\n***********************************\n");
	_atomicList.reverseSweep(f);
}