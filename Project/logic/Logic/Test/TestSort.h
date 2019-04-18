#ifndef __Test_Sort_h__
#define __Test_Sort_h__
#include "IKernel.h"


struct Node
{
	s32 id;
	Node *_next;

	void Print(core::IKernel *kernel)
	{
		TRACE_LOG("node id:%d", id);
	}
};

class TestSort
{
public:
	static void Test(core::IKernel *pKernel);

	static Node * GetNewNode(s32 id);
	static void Print(Node *head, core::IKernel *kernel);

protected:
private:
};

#endif
