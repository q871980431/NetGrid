#ifndef __TestMatch_h__
#define __TestMatch_h__
#include "IKernel.h"
#include "MultiSys.h"
#include "ISearch.h"
#include <unordered_map>

class ISearch;
using namespace core;
class TestMatch
{
public:
	TestMatch() {};
	static void Test(core::IKernel *kernel);

	static void TestIndividualSearch();

private:
	static core::IKernel *s_kernel;
	static ISearch *s_search;
};

class MatchTask	: public ITimer
{
	struct  PlayerInfo
	{
		s64 id;
		s32 score;
		s64 matchTick;
		bool match;

		s32 count;
	};
	typedef std::unordered_map<s64, PlayerInfo> PlayerMap;
public:
	MatchTask(s32 taskId, ISearch *search, ISearch::SearchConfig &config);
	~MatchTask();

	bool Initialize( IKernel *kernel);

	virtual void OnStart(IKernel *kernel, s64 tick) {};
	virtual void OnTime(IKernel *kernel, s64 tick);
	virtual void OnTerminate(IKernel *kernel, s64 tick) {};
	PlayerInfo * GetPlayerInfo( s64 playerId);
	void Remove(s64 playerId);
	void AddPlayerToMatch();

	void TestFunction();
protected:
private:
	ISearch		*_search;
	IKernel		*_kernel;

	s32	_taskId;
	ISearch::SearchConfig _config;
	PlayerMap	_players;
	s64			_channelId;
	s32			_count;
	s32			_id;
};

#endif