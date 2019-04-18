#include "TestMatch.h"
#include "ISearch.h"
#include "Tools.h"
#include "Tools_time.h"
#include <map>
#include <algorithm>
#include <vector>

core::IKernel * TestMatch::s_kernel = nullptr;
ISearch * TestMatch::s_search = nullptr;
s64 *g_ids = nullptr;
s32 *g_scores = nullptr;

void TestMatch::Test(core::IKernel *kernel)
{
	s_kernel = kernel;
	//FIND_MODULE(s_search, Search);

    std::map<s32, s32, std::greater<s32>> playerScore;
    for (s32 i = 0; i < 10; i++)
    {
        playerScore.insert(std::make_pair(i, i));
    }

	TestIndividualSearch();
}

void MatchBack(s32 count, const ISearch::SetInfo *setInfo)
{
	s32 static s_count = 0;

	ECHO("Count = %d, match count = %d", ++s_count, count);
	for (s32 i = 0; i < count; i++)
	{
		ECHO("team[%2d] ,count = %d", i, setInfo[i].num);
		for (s32 j = 0; j < setInfo[i].num; j++)
		{
			s32 offset = setInfo[i].id[j] - 1;
			ECHO("id = %2lld, score = %2d", g_ids[offset], g_scores[offset]);
		}
	}
}

void TestMatch::TestIndividualSearch()
{
	s32 num = 250;
	s32 temp = -123;
	if (temp)
	{
		printf("is true");
	}
	ISearch::SearchConfig config;
	config.min = 1;
	config.max = 20;
	config.cellVal = 3;
	config.num = 5;
	config.numMin = 2;
	config.reduce = 1;

	config.model = ISearch::SEARCH_MODEL_PARTNER;
	//config.model = ISearch::SEARCH_MODEL_PAIR_GAME;
	config.transfDelay = 1000;
	config.searchInterval = 1000;
	config.transfRadio = 1;
	config.transfModel = 1;
	config.transfMax = 10;
	config.calBack = MatchBack;

	for (s32 i = 0; i < 1; i++)
	{
		config.min = tools::GetRandom(1, 1);
		config.max = tools::GetRandom(config.min, config.min + 0);
		config.numMin = tools::GetRandom(5, 10);
		config.num = tools::GetRandom(config.numMin, config.numMin + 5);
		config.numMin = 30;
		config.num = 50;
		MatchTask *task = NEW MatchTask(i, s_search, config);
		task->Initialize(s_kernel);
	}
}


MatchTask::MatchTask(s32 taskId, ISearch *search, ISearch::SearchConfig &config):_search(search), _config(config)
{
	_channelId = 0;
	_taskId = taskId;
	_id = false;
	//TestFunction();
}

MatchTask::~MatchTask()
{
	if (_kernel != nullptr)
	{
		_kernel->KillTimer(this);
		_search->DelSearchChannel(_channelId);
	}
}


bool MatchTask::Initialize(IKernel *kernel)
{
	_kernel = kernel;
	if (_kernel == nullptr)
		return false;
	ISearch::SearchConfig config = _config;
	MatchTask *selef = this;
	auto f = [config, selef](s32 count, const ISearch::SetInfo *setInfo)
	{
		for (s32 i = 0; i < count; i++)
		{
			ASSERT(setInfo[i].num >= config.numMin, "error");
			printf("\n*******************[%d], Num:%d***************** \n", i, setInfo[i].num);
			for (s32 j = 0; j < setInfo[i].num; j++)
			{
				s64 id = setInfo[i].id[j];
				PlayerInfo *info = selef->GetPlayerInfo(id);
				ASSERT(info != nullptr, "error");
				ASSERT(info->match == false, "error");
				info->match = true;
				printf("PlayerId:%lld, Score:%d     ", info->id, info->score);
				if (info->count >= 1)
				{
					selef->Remove(id);
				}
			}
		}
	};
	_config.calBack = f;
	ECHO("NumMax:%d, NumMin:%d", _config.num, _config.numMin);
	_channelId = _search->RegisterSearchChannel(&_config);
	if (_channelId == 0)
		return false;
	_count = tools::GetRandom(_config.numMin, _config.numMin);
	s32 init = tools::GetRandom(1, _count);
	for (s32 i = 0; i < init; i++)
	{
		AddPlayerToMatch();
	}
	_kernel->StartTimer(this, 1000, FOREVER, 2000, "match task");
	return true;
}

void MatchTask::OnTime(IKernel *kernel, s64 tick)
{
	s32 enterMin = 60;
	s32 enterMax = 80;
	if (_id < _count)
	{
		s32 tmp = tools::GetRandom(enterMin, enterMax);
		for (s32 i = 0; i < tmp; i++)
			AddPlayerToMatch();
		tmp = tools::GetRandom(1, 5);
		for (s32 i = 0; i < tmp; i++)
		{
			s32 randomId = tools::GetRandom(1, _id);
			//_search->LeaveIndividualSearchChannel(_channelId, randomId);
		}
		DEBUG_LOG("Task %d, Match Count = %lld", _taskId, _players.size());
	}
	else
	{
		if (_players.size() > 0)
		{
			s32 tmp = tools::GetRandom(enterMin, enterMax);
			for (s32 i = 0; i < tmp; i++)
				AddPlayerToMatch();
		}
		else
		{
			DEBUG_LOG("*******************Complete Search Test*******************");
		}

	}

	//DEL this;
}

MatchTask::PlayerInfo * MatchTask::GetPlayerInfo(s64 playerId)
{
	auto iter = _players.find(playerId);
	return iter == _players.end() ? nullptr : &(iter->second);
}

void MatchTask::Remove(s64 playerId)
{
	auto iter = _players.find(playerId);
	if (iter != _players.end())
	{
		_players.erase(iter);
	}
}
void MatchTask::AddPlayerToMatch()
{
	if (_id < _count)
	{
		++_id;
		PlayerInfo playerInfo;
		playerInfo.id = _id;
		playerInfo.matchTick = tools::GetTimeMillisecond();
		playerInfo.score = tools::GetRandom(_config.min, _config.max);
		playerInfo.match = false;
		playerInfo.count = 0;

		_search->EnterIndividualSearchChannel(_channelId, playerInfo.id, playerInfo.score);
		auto ret = _players.insert(std::make_pair(playerInfo.id, playerInfo));
		ASSERT(ret.second, "error");
	}
	else
	{
		for (auto &playerInfo : _players)
		{
			if (playerInfo.second.match == true)
			{
				_search->EnterIndividualSearchChannel(_channelId, playerInfo.second.id, playerInfo.second.score);
				playerInfo.second.match = false;
				playerInfo.second.count += 1;
				return;
			}
		}
	}
}

void MatchTask::TestFunction()
{
	typedef std::vector<s32> NumWarper;
	const char *a = "123456";
	const char *b = "456";
	auto f = [](const char *s, const char *b)
	{
		s32 lenS = strlen(s);
		s32 lenB = strlen(b);
		int *ret = NEW int[lenS + 1];
		memset(ret, 0, sizeof(int) *(lenS + 1));
		for (s32 i = 0; i < lenS; i++)
		{
			s32 tmpS = s[lenS-1-i] - '0';
			s32 tmpB = 0;
			if (i < lenB)
				tmpB = (b[lenB - 1 - i] - '0');
			ret[i] += tmpS + tmpB ;
			if (ret[i] >= 10)
			{
				ret[i] -= 10;
				ret[i+1] += 1;
			}
		}
		for (s32 i = lenS; i >= 0; i--)
		{
			if (i == lenS)
			{
				if(ret[lenS] != 0)
					printf("%d", ret[i]);
			}
			else
				printf("%d", ret[i]);
		}
		DEL[] ret;
	}; 
	if (strlen(a) > strlen(b))
		f(a, b);
	else
		f(b, a);
	NumWarper numbers;
}