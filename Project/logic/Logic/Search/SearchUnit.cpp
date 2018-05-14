/*
 * File:	Search.cpp
 * Author:	xuping
 * 
 * Created On 2018/1/2 20:32:57
 */

#include "SearchUnit.h"
#include "Tools.h"
#include "IKernel.h"
#include<algorithm> 
#define TASSERT	ASSERT

SearchUnit::SearchUnit(core::IKernel *kernel, s64 id, const ISearch::SearchConfig *config) :_id(id), _config(*config)
{
	_kernel = kernel;
	_expandCount = 0;
	_flushCount = 0;
	_update = false;
	_transforTimer = NEW TransforTimer(this);
	_flushSearchTimer = NEW FlushSearchTimer(this);
	_kernel->StartTimer(_transforTimer, _config.transfDelay, FOREVER, _config.transfDelay, "transfer timer");
	_kernel->StartTimer(_flushSearchTimer, _config.searchInterval, FOREVER, _config.searchInterval, "flush search timer");
	for (s32 i = 0; i < TEAM_COUNT; i++)
		_matchIds[i] = NEW s64[_config.num];
}

SearchUnit::~SearchUnit()
{
	_kernel->KillTimer(_transforTimer);
	_kernel->KillTimer(_flushSearchTimer);
	DEL _transforTimer;
	DEL _flushSearchTimer;
	for (s32 i = 0; i < TEAM_COUNT; i++)
		DEL[]_matchIds[i];
	for (auto iter : _individualMap)
		DEL iter.second;
}

bool SearchUnit::EnterIndividualSearch(s64 personId, s32 val)
{
	auto iter = _individualMap.find(personId);
	if (iter != _individualMap.end())
		return false;
	IndividualInfo *info = NEW IndividualInfo();
	InitSearchArgs(&info->searchArgs, personId, val);
	_individual.push_back(info);
	_update = true;

	auto ret = _individualMap.insert(std::make_pair(personId, info));
	return ret.second;
}

bool SearchUnit::EnterTeamSearchChannel(s64 teamId, s32 val, s32 num, s64 *memberId)
{
	return true;
}

bool SearchUnit::LeaveIndividualSearchChannel(s64 personId)
{
	auto iter = _individualMap.find(personId);
	//TASSERT(iter != _individualMap.end(), "error");
	if (iter != _individualMap.end())
	{
		for (auto listIter = _individual.begin(); listIter != _individual.end(); listIter++)
		{
			if ((*listIter)->searchArgs.id == personId)
			{
				_individual.erase(listIter);
				break;
			}
		}
		DEL iter->second;
		_individualMap.erase(iter);
		return true;
	}

	return false;
}

bool SearchUnit::LeaveTeamSearchChannel(s64 teamId)
{
	return true;
}

void SearchUnit::ValExpand()
{
	++_expandCount;
	for ( auto iter : _individual)
	{
		if (_config.numMin != 0 && iter->searchArgs.num > _config.numMin)
		{
			if (_config.model == ISearch::SEARCH_MODEL_PAIR_GAME)
			{
				if (iter->searchArgs.num <= _config.numMin * 2)
					continue;
			}

			iter->searchArgs.num = _config.num - (s32)(_config.reduce * iter->searchArgs.deep);
			if (iter->searchArgs.num < _config.numMin)
				iter->searchArgs.num = _config.numMin;
			if (_config.model == ISearch::SEARCH_MODEL_PAIR_GAME)
			{
				iter->searchArgs.num *= 2;
			}
			_update = true;
		}

		if (iter->searchArgs.deep < _config.transfMax)
			SearchExpand(&iter->searchArgs);
	}
}

bool SearchCmp(const IndividualInfo *left, const IndividualInfo *right)
{
	return left->searchArgs.val < right->searchArgs.val;
}

bool SearchCmp1(const IndividualInfo *left, const IndividualInfo *right)
{
	return left->searchArgs.begin < right->searchArgs.begin;
}


void SearchUnit::FlushSearch()
{
	if (!_update)
		return;

	++_flushCount;
	_individual.sort(SearchCmp1);

	auto start = _individual.begin();
	auto end = _individual.end();
	s32 mark = 0;
	s32 size = (s32)_individual.size();

	printf("******************FlushSearch Size:%d*************\n", size);
	s32 offset = _config.numMin / 2;
	for (auto iter = start; iter != end; iter++, mark++)
	{
		if (mark < offset)
			continue;

		IndividualMatch tmp;
		tmp.push_back(*iter);
		SearchBox box;
		box.num = (*iter)->searchArgs.num;
		box.searchBegin = (*iter)->searchArgs.begin;
		box.searchEnd = (*iter)->searchArgs.end;
		box.valBegin = (*iter)->searchArgs.val;
		box.valEnd = (*iter)->searchArgs.val;

		if (!(*iter)->searchArgs.remove)
		{
			s32 lastMark = mark;
			s32 nextMark = size - mark - 1;
			auto last = iter;
			auto next = iter;
			while (lastMark > 0 || nextMark > 0)
			{
				if (lastMark > 0)
				{
					--last;
					--lastMark;
					if (!(*last)->searchArgs.remove)
					{
						if ((*last)->searchArgs.end < box.searchBegin)
						{
							lastMark = 0;
						}
						else
						{
							s32 num = box.num;
							if (InsertBox(&box, (*last)->searchArgs.begin, (*last)->searchArgs.val, (*last)->searchArgs.end, (*last)->searchArgs.num))
							{
								tmp.push_back(*last);
								num = _config.num;
							}
							if (tmp.size() >= num)
							{
								OnMatchSuccess(&tmp);
								tmp.clear();
								break;
							}
						}
					}
				}

				if (nextMark > 0)
				{
					++next;
					--nextMark;

					if (!(*next)->searchArgs.remove)
					{
						if ((*next)->searchArgs.begin > box.searchEnd)
						{
							nextMark = 0;
						}
						else
						{
							s32 num = box.num;
							if (InsertBox(&box, (*next)->searchArgs.begin, (*next)->searchArgs.val, (*next)->searchArgs.end, (*next)->searchArgs.num))
							{
								tmp.push_back(*next);
								num = _config.num;
							}
							if (tmp.size() >= num)
							{
								OnMatchSuccess(&tmp);
								tmp.clear();
								break;
							}
						}
					}
				}
			}

			if (tmp.size() >= box.num)
			{
				OnMatchSuccess(&tmp);
				break;
			}
		}
	}

	for (auto iter = _individual.begin(); iter != _individual.end();)
	{
		if ((*iter)->searchArgs.remove)
			iter = _individual.erase(iter);
		else
			iter++;
	}

	for (auto iter = _individualMap.begin(); iter != _individualMap.end(); )
	{
		if (iter->second->searchArgs.remove)
		{
			DEL iter->second;
			iter = _individualMap.erase(iter);
		}
		else
			iter++;
	}

	_update = false;
}

void SearchUnit::InitSearchArgs(SearchArgs *searchArgs, s64 id, s32 val)
{
	searchArgs->id = id;
	searchArgs->val = val;
	searchArgs->deep = 0;
	searchArgs->num = _config.num;
	if (_config.model == ISearch::SEARCH_MODEL_PAIR_GAME)
		searchArgs->num *= 2;

	s32 index = (val - _config.min) / _config.cellVal;
	if (index < 1)
		searchArgs->begin = _config.min;
	else
		searchArgs->begin = _config.min + index * _config.cellVal;

	searchArgs->end = searchArgs->begin + _config.cellVal - 1;
}

void SearchUnit::SearchExpand(SearchArgs *searchArgs)
{
	if (searchArgs->begin != _config.min)
	{
		_update = true;
		searchArgs->begin -= (_config.transfRadio * _config.cellVal);
		if (searchArgs->begin < _config.min)
			searchArgs->begin = _config.min;
	}

	if (searchArgs->end != _config.max)
	{
		_update = true;
		searchArgs->end += (_config.transfRadio * _config.cellVal);
		if (searchArgs->end > _config.max)
			searchArgs->end = _config.max;
	}
	
	searchArgs->deep++;
}

void SearchUnit::OnMatchSuccess(IndividualMatch *matchs)
{
	s32 num = 0;
	if (_config.model == ISearch::SEARCH_MODEL_PAIR_GAME)
	{
		s32 tmp = matchs->size() % 2;
		TASSERT(tmp == 0, "error");
		num = (s32)(matchs->size() / 2);
		TASSERT(num > 0, "error");
		if (num >0)
		{
			TASSERT(num <= _config.num, "error");
			if (num > _config.num)
				num = _config.num;

			ISearch::SetInfo set[TEAM_COUNT];
			set[TEAM_RED].id = _matchIds[TEAM_RED];
			set[TEAM_BLACK].id = _matchIds[TEAM_BLACK];
			auto back = matchs->end();
			--back;
			s32 mark = 0;
			for (auto iter = matchs->begin(); iter != matchs->end()&& mark < (2 * num); iter++, mark++)
			{
				s32 offset = mark / num;
				s32 tmp = mark % num;
				set[offset].id[tmp] = (*iter)->searchArgs.id;

				(*iter)->searchArgs.remove = true;
			}

			set[TEAM_RED].num = num;
			set[TEAM_BLACK].num = num;

			_config.calBack(TEAM_COUNT, set);
		}
	}
	else
	{
		num = matchs->size();
		if (num > 0)
		{
			TASSERT(num <= _config.num, "error");
			TASSERT(num >= _config.numMin, "error");
			if (num > _config.num)
				num = _config.num;

			ISearch::SetInfo set;
			set.id = _matchIds[TEAM_RED];
			s32 mark = 0;
			for (auto iter = matchs->begin(); iter != matchs->end() && mark < num; iter++)
			{
				set.id[mark++] = (*iter)->searchArgs.id;
				(*iter)->searchArgs.remove = true;
			}

			set.num = num;
			_config.calBack(1, &set);
		}
	}
}

bool SearchUnit::InsertBox(SearchBox *box, s32 begin, s32 val, s32 end, s32 num)
{
	s32 andBegin = (std::max)(box->searchBegin, begin);
	s32 andEnd = (std::min)(box->searchEnd, end);
	if (andBegin > andEnd)
		return false;
	bool inner = andBegin <= box->valBegin && box->valEnd <= andEnd;
	if (!inner)
		return false;
	inner = andBegin <= val && val <= andEnd;
	if (!inner)
		return false;
	if (box->num < num)
		box->num = num;
	if (val < box->valBegin)
		box->valBegin = val;
	else if ( val > box->valEnd)
	{
		box->valEnd = val;
	}
	box->searchBegin = andBegin;
	box->searchEnd = andEnd;
	return true;
}