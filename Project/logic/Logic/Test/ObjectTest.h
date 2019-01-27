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
class Parent {

public:

	int iparent;

	Parent() :iparent(10) {}

	virtual void f() { ECHO(" Parent::f()"); }

	virtual void g() { ECHO(" Parent::g()"); }

	virtual void h() { ECHO(" Parent::h()"); }
};

class Child : public Parent {

public:

	int ichild;

	Child() :ichild(100) {}

	virtual void f() { ECHO("Child::f()"); }

	virtual void g_child() { ECHO("Child::g_child()"); }

	virtual void h_child() { ECHO("Child::h_child()"); }

};




class GrandChild : public Child {

public:

	int igrandchild;

	GrandChild() :igrandchild(1000) {}

	virtual void f() { ECHO("GrandChild::f()"); }

	virtual void g_child() { ECHO("GrandChild::g_child()"); }

	virtual void h_grandchild() { ECHO("GrandChild::h_grandchild()"); }

};


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