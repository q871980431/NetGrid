/*
 * File:	LeetCode.h
 * Author:	XuPing
 * 
 * Created On 2019/4/17 16:46:43
 */

#ifndef __SkipList_h__
#define __SkipList_h__

#include "MultiSys.h"
#include <vector>
#include <unordered_map>

namespace tlib
{
	struct IRankNode;
	struct RankLevel
	{
		IRankNode	*_next;
		u32			span;
	};
	struct IRankNode
	{
		s64			_score;
		IRankNode	*_pre;
		RankLevel	*_level;
		s64			_id;
	};
	class IRankList
	{
	public:
		const static s32 RANK_MAX_LEVEL = 32;
		const static  u32 SKIPLIST_P = (0.25 * 0xFFFF);

		IRankList(s32 level = RANK_MAX_LEVEL);
		~IRankList() { Clean(); };

		IRankNode * Insert(s64 score, s64 id);
		bool		Delete(s64 score, s64 id);
		IRankNode * GetRankNodeByRank(u32 rank);
		s32			GetRank(s64 score, s64 id);
		void		Printf();

	private:
		IRankNode * CreateNode(s32 level, s64 score);
		void Clean();
		void ReleaseNode(IRankNode *node);
		s32 RandomLevel();
		void DeleteNode(IRankNode *x, IRankNode **update);

	private:
		IRankNode *_header;
		IRankNode *_tail;
		s32	_length;
		s32	_level;
		s32 _maxLevel;
	};

}

#endif