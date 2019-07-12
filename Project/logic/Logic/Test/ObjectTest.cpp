/*
 * File:	ObjectTest.cpp
 * Author:	XuPing
 * 
 * Created On 2018/2/11 4:19:22
 */

#include "ObjectTest.h"
#include <string>

bool ObjectTest::Initialize(IKernel *kernel)
{
	
	typedef void(*Fun)(void);
	GrandChild gc;
	s64** pVtab = (s64**)&gc;
	Fun pFun = nullptr;
	ECHO("[0] GrandChild::_vptr->");

	for (int i = 0; pVtab[0][i] != 0; i++) {

		pFun = (Fun)pVtab[0][i];

		ECHO("    [%d] ", i);
		pFun();

	}

	ECHO("[1] Parent.iparent = %d",(int)*pVtab[1]);

	ECHO("[2] Child.ichild = %d",(int)*pVtab[2]);

	ECHO("[3] GrandChild.igrandchild = %d",(int)*pVtab[3]);

    return true;
}


