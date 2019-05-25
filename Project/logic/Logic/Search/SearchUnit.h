/*
 * File:	Search.h
 * Author:	xuping
 * 
 * Created On 2018/1/2 20:32:57
 */

#ifndef __SearchUnit_h__
#define __SearchUnit_h__
#include "Search.h"
#include <list>

struct SearchArgs
{
	s64 id;

	s8 deep;
	s32 val;					//所处值
	s32 begin;					//下限值
	s32 end;					//上限值

	s32 num;					//准入集合数
	bool remove;				//移除			
};

struct IndividualInfo
{
	SearchArgs searchArgs;
};

struct TeamInfo 
{
	SearchArgs searchArgs;
	s32 num;
	s64 *ids;
};

struct SearchBox 
{
	s32 searchBegin;
	s32 searchEnd;
	s32 valBegin;
	s32 valEnd;

	s32 num;
};


typedef std::list<IndividualInfo *> IndividualMatch;
typedef std::unordered_map<s64, IndividualInfo *> IndividualMap;
typedef std::list<TeamInfo *> TeamMatch;
typedef std::unordered_map<s64, TeamInfo *> TeamMap;
class core::IKernel;
class TransforTimer;
class FlushSearchTimer;
class SearchUnit
{
	enum
	{
		TEAM_RED = 0,
		TEAM_BLACK = 1,
		TEAM_COUNT,
	};
public:
	SearchUnit( core::IKernel *kernel, s64 id, const ISearch::SearchConfig *config);
	~SearchUnit();

	bool EnterIndividualSearch(s64 personId, s32 val);
	bool EnterTeamSearchChannel(s64 teamId, s32 val, s32 num, s64 *memberId);
	bool LeaveIndividualSearchChannel(s64 personId);
	bool LeaveTeamSearchChannel(s64 teamId);

	void ValExpand();
	void FlushSearch();
protected:
	void InitSearchArgs(SearchArgs *searchArgs, s64 id, s32 val);
	void SearchExpand(SearchArgs *searchArgs);
	void OnMatchSuccess(IndividualMatch *matchs);

	bool InsertBox(SearchBox *box, s32 begin, s32 val, s32 end, s32 num);
private:
	s64	_id;
	ISearch::SearchConfig _config;
	core::IKernel	*_kernel;

	IndividualMatch	_individual;
	IndividualMap	_individualMap;
	TransforTimer   * _transforTimer;
	FlushSearchTimer * _flushSearchTimer;

	s32  _expandCount;
	s32  _flushCount;
	bool _update;
	s64	*_matchIds[TEAM_COUNT];
};

class TransforTimer : public core::ITimer
{
public:
	TransforTimer(SearchUnit *searchUnit) :_searchUnit(searchUnit) {};
	virtual ~TransforTimer() {};

	virtual void OnStart(IKernel *kernel, s64 tick) {};
	virtual void OnTime(IKernel *kernel, s64 tick) { _searchUnit->ValExpand(); };
	virtual void OnTerminate(IKernel *kernel, s64 tick, bool isKill) {};
protected:
private:
	SearchUnit *_searchUnit;
};

class FlushSearchTimer : public core::ITimer
{
public:
	FlushSearchTimer(SearchUnit *searchUnit) :_searchUnit(searchUnit) {};
	virtual ~FlushSearchTimer() {};

	virtual void OnStart(IKernel *kernel, s64 tick) {};
	virtual void OnTime(IKernel *kernel, s64 tick) { _searchUnit->FlushSearch(); };
	virtual void OnTerminate(IKernel *kernel, s64 tick, bool isKill) {};
protected:
private:
	SearchUnit *_searchUnit;
};
#endif