/*
 * File:	Search.cpp
 * Author:	xuping
 * 
 * Created On 2018/1/2 20:32:57
 */

#include "Search.h"
#include "SearchUnit.h"
#define TASSERT	ASSERT
Search * Search::s_self = nullptr;
IKernel * Search::s_kernel = nullptr;
s64		 Search::s_guid = 0;
Search::SearchMap Search::s_serachMap;
bool Search::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool Search::Launched(IKernel *kernel)
{

    return true;
}

bool Search::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

s64 Search::RegisterSearchChannel(const SearchConfig *config)
{
	TASSERT(config, "error");
	if (config->model <= SEARCH_MODEL_MIN || config->model >= SEARCH_MODEL_MAX)
		return 0;
	if (config->num <= 1)
		return 0;

	s64 id = ++s_guid;
	SearchUnit *unit = NEW SearchUnit(s_kernel, id, config);
	auto ret = s_serachMap.insert(std::make_pair(id, unit));
	if (!ret.second)
		return 0;
	return id;
}

bool Search::DelSearchChannel(s64 channelId)
{
	auto iter = s_serachMap.find(channelId);
	if (iter != s_serachMap.end())
	{
		DEL iter->second;
		s_serachMap.erase(iter);
	}

	return true;
}

bool Search::EnterIndividualSearchChannel(s64 channelId, s64 personId, s32 val)
{
	auto iter = s_serachMap.find(channelId);
	if (iter != s_serachMap.end())
	{
		iter->second->EnterIndividualSearch(personId, val);
	}

	return true;
}

bool Search::EnterTeamSearchChannel(s64 channelId, s64 teamId, s32 val, s32 num, s64 *memberId)
{
	return true;
}

bool Search::LeaveIndividualSearchChannel(s64 channelId, s64 personId)
{
	auto iter = s_serachMap.find(channelId);
	if (iter != s_serachMap.end())
	{
		iter->second->LeaveIndividualSearchChannel(personId);
	}

	return true;
}

bool Search::LeaveTeamSearchChannel(s64 channelId, s64 teamId)
{
	return true;
}