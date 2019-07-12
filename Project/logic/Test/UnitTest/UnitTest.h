/*
 * File:	UnitTest.h
 * Author:	XuPing
 * 
 * Created On 2019/5/16 16:19:20
 */

#ifndef __UnitTest_h__
#define __UnitTest_h__
#include "IUnitTest.h"
#include <vector>

typedef std::vector<IUnitTestInstance *> TestList;
class UnitTest : public IUnitTest
{
public:
    virtual ~UnitTest(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

	static TestList &GetTestList() { return s_testList; };
protected:
private:
    static UnitTest     * s_self;
    static IKernel  * s_kernel;
	static TestList		s_testList;
};

template<typename T>
struct UnitTestRegister
{
	UnitTestRegister()
	{
		printf("this UnitTest Register 1\n");
		TestList &list = UnitTest::GetTestList();
		list.push_back(NEW T());
		printf("list size:%ld", list.size());
	}
};
#endif
