#ifndef __Test_Rtree_h__
#define __Test_Rtree_h__

#include "IKernel.h"
#include "RTree.h"
#include "Vector.h"
#include <map>

struct TestNode 
{
	s32 id;
	s32 querySize;
};

struct QueryNode 
{
	s32 count;
	s32 querySize;
};

typedef RTree<TestNode*, s32, 2, s32, 48, 32>    SearchTree;

class TestRtree
{
public:
	TestRtree() {};
	~TestRtree() {};

	static void Test(core::IKernel *kernel);
protected:

	void BuildTestQueryTask(core::IKernel *kernel, s32 size);
	void InitTreeEnity(SearchTree *tree, s32 size);
	s32  QueryTree(SearchTree *tree, MBRect<> &rect, s32 querySize, s32 &searched);
private:
	inline s32 GetId() { return ++_id; };
private:

	static core::IKernel *s_kernel;
	s32		_id;
	std::map<s32, TestNode>	_nodes;

};

#endif
