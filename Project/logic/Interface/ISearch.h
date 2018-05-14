/*
 * File:	ISearch.h
 * Author:	xuping
 * 
 * Created On 2018/1/2 20:32:57
 */

#ifndef  __ISearch_h__
#define __ISearch_h__
#include "IModule.h"
#include <functional>

class ISearch : public IModule
{
public:
	enum 
	{
		SEARCH_MODEL_MIN = 0,
		SEARCH_MODEL_PARTNER = 1,		//搜寻伙伴
		SEARCH_MODEL_PAIR_GAME = 2,		//搜寻双打
		SEARCH_MODEL_MAX,
	};

	enum 
	{
		SEARCH_TRANSF_MIN = 0,
		SEARCH_TRANSF_LINEAR = 1,				//线性变化
		SEARCH_TRANSF_LINEAR_ACCELERATION = 2,	//线性加速
		SEARCH_TRANSF_MAX,
	};

	const static s32 SEARCH_INTERVAL_MIN = 100;		//搜寻最低间隔时间
	const static s32 TRANSF_INTERVAL_MIN = 1000;	//变换最低间隔时间

	struct SetInfo
	{
		s32 num;
		s64 *id;
	};

	typedef std::function<void(s32 count, const SetInfo *setInfo)> SearchCallBack;

	struct  SearchConfig
	{
		s32 min;			//最低分值
		s32 max;			//最大分值
		s32 cellVal;		//单元值

		s8 num;				//集合人数
		s8 numMin;			//集合最低人数，>0 则根据reduce 进行每轮降低 可进入集合人数
		float reduce;		//减少值
		s8 model;			//搜寻模式

		s32 searchInterval; //搜寻间隔	ms
		s32 transfDelay;	//变换时延	ms
		s32 transfRadio;	//变换半径	2
		s32 transfModel;    //变换模型
		s32 transfMax;		//变换最大次数
		SearchCallBack calBack; //回调函数
	};


public:
    virtual ~ISearch(){};
    
	virtual s64 RegisterSearchChannel(const SearchConfig *config) = 0;
	virtual bool DelSearchChannel(s64 channelId) = 0;
	virtual bool EnterIndividualSearchChannel(s64 channelId, s64 personId, s32 val) = 0;
	virtual bool EnterTeamSearchChannel(s64 channelId, s64 teamId, s32 val, s32 num, s64 *memberId) = 0;
	virtual bool LeaveIndividualSearchChannel(s64 channelId, s64 personId) = 0;
	virtual bool LeaveTeamSearchChannel(s64 channelId, s64 teamId) = 0;
};
#endif