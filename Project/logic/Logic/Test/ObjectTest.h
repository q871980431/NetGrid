/*
 * File:	ObjectTest.h
 * Author:	XuPing
 * 
 * Created On 2018/2/11 4:19:22
 */

#ifndef __ObjectTest_h__
#define __ObjectTest_h__
#include "IKernel.h"
using namespace core;
class ObjectTest
{
public:
	ObjectTest(IKernel *kernel) :_kernel(kernel) {};
    virtual ~ObjectTest(){};

    bool Initialize(IKernel *kernel);
protected:
private:
    IKernel  * _kernel;
};
#endif