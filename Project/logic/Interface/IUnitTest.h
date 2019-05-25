/*
 * File:	IUnitTest.h
 * Author:	XuPing
 * 
 * Created On 2019/5/16 16:19:20
 */

#ifndef  __IUnitTest_h__
#define __IUnitTest_h__
#include "IModule.h"
class IUnitTest : public IModule
{
public:
    virtual ~IUnitTest(){};
    
};

class IUnitTestInstance
{
public:
	virtual ~IUnitTestInstance() {};
	virtual void StartTest(core::IKernel *kernel) = 0;
};

#endif