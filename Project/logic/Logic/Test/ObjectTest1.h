/*
 * File:	ObjectTest.h
 * Author:	XuPing
 * 
 * Created On 2018/2/11 4:19:22
 */

#ifndef __ObjectTest_h__
#define __ObjectTest_h__
//#include "IKernel.h"
//using namespace core;

class Parent {

public:

	int iparent;

	Parent() :iparent(10) {}

	virtual void f() {  }

	virtual void g() { }

	virtual void h() { }
};

class Child : public Parent {

public:

	int ichild;

	Child() :ichild(100) {}

	virtual void f() { }

	virtual void g_child() { }

	virtual void h_child() { }

};




class GrandChild : public Child {

public:

	int igrandchild;

	GrandChild() :igrandchild(1000) {}

	virtual void f() { }

	virtual void g_child() { }

	virtual void h_grandchild() { }

};

#endif