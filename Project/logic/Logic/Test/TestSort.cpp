#include "TestSort.h"

void TestSort::Test(core::IKernel *kernel)
{
	Node *head = nullptr;
	for (s32 i = 0; i < 10; i++)
	{
		Node *tmp = GetNewNode(i);
		if (head == nullptr)
			head = tmp;
		else
		{
			Node *tail = head;
			while (tail->_next)
			{
				tail = tail->_next;
			}
			tail->_next = tmp;
		}
	}
	Print(head, kernel);
	TRACE_LOG("Satrt re sort");
	Node *tail = head;
	Node *newHead = nullptr;
	while (tail)
	{
		Node *now = tail;
		tail = tail->_next;
		if (newHead == nullptr)
		{
			newHead = now;
			newHead->_next = nullptr;
		}
		else
		{
			now->_next = newHead;
			newHead = now;
		}
	}
	Print(newHead, kernel);
	tail = newHead;
	while (tail)
	{
		Node *now = tail;
		tail = tail->_next;
		DEL now;
	}
}

Node * TestSort::GetNewNode(s32 id)
{
	Node *ret = NEW Node();
	ret->id = id;
	return ret;
}

void TestSort::Print(Node *head, core::IKernel *kernel)
{
	Node *tail = head;
	while (tail)
	{
		tail->Print(kernel);
		tail = tail->_next;
	}
}