#include "TestRtree.h"
#include "Tools.h"
#include "Tools_time.h"

void TestRtree::Test(core::IKernel *kernel)
{
	TestRtree tree;
	tree.BuildTestQueryTask(kernel, 100);
	tree.BuildTestQueryTask(kernel, 1000);
	tree.BuildTestQueryTask(kernel, 2000);
	tree.BuildTestQueryTask(kernel, 4000);
	tree.BuildTestQueryTask(kernel, 8000);
	tree.BuildTestQueryTask(kernel, 10000);
}

void TestRtree::BuildTestQueryTask(core::IKernel *kernel, s32 size)
{
	SearchTree searchTree;
	InitTreeEnity(&searchTree, size);
	MBRect<> queryRect;
	queryRect.left.x[VectorDefine::X] = tools::GetRandom(2, 5);
	queryRect.left.x[VectorDefine::Y] = tools::GetRandom(2, 5);
	queryRect.right = queryRect.left;
	queryRect.right + 3;
	s32 count = 10;
	s32 time = 0;
	s32 searchCount = 0;
	for (s32 i = 0; i < count; i++)
	{
		s32 pertime = QueryTree(&searchTree, queryRect, 10, searchCount);
		time += pertime;
	}
	time /= count;
	ECHO("Tree size:%d, query 10, expends time:%d ns", size, time);
}

void TestRtree::InitTreeEnity(SearchTree *tree, s32 size)
{
	for (s32 i = 0; i < size; i++)
	{
		s32 id = GetId();
		s32 pos[2] = { 0 };
		pos[0] = tools::GetRandom(1, 10);
		pos[1]  = tools::GetRandom(1, 10);
		auto ret = _nodes.emplace(id, TestNode{ id, 0 });
		s32 posR[2];
		posR[0] = pos[0]+1;
		posR[1] = pos[1]+1;

		tree->Insert(pos, posR, &(ret.first->second));
	}
}

bool QueryCallBack(TestNode *node, void *context)
{
	QueryNode *queryPrt = (QueryNode*)context;
	if (queryPrt->count < queryPrt->querySize)
	{
		queryPrt->count++;
		return true;
	}
	return false;
}

s32 TestRtree::QueryTree(SearchTree *tree, MBRect<> &rect, s32 querySize, s32 &searched)
{
	s64 startTick = tools::GetTimeNanosecond();
	QueryNode node;
	node.count = 0;
	node.querySize = querySize;
	tree->Search(rect.left.x, rect.right.x, QueryCallBack, &node);
	searched = node.count;
	return (s32)(tools::GetTimeNanosecond() - startTick);
}