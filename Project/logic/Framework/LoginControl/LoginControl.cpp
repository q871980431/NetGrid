/*
 * File:	LoginControl.cpp
 * Author:	xuping
 * 
 * Created On 2019/7/8 11:43:21
 */

#include "LoginControl.h"
#include "XmlReader.h"
#include "Tools_time.h"

LoginControl	* LoginControl::s_self = nullptr;
IKernel			* LoginControl::s_kernel = nullptr;
WaitConfigInfo	  LoginControl::s_waitConfig;
WaitServiceInfo   LoginControl::s_waitInfo;
WaitAccountMap	  LoginControl::s_waitAccount;
TickMap			  LoginControl::s_tickIndex;

bool LoginControl::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	if (!LoadConfigFile(kernel))
		return false;
	s_waitInfo.rateNum = s_waitConfig.rateMax;
    
    return true;
}

bool LoginControl::Launched(IKernel *kernel)
{
	s_kernel->StartTimer(this, 0, FOREVER, 1000, "login control");
    return true;
}

bool LoginControl::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

void LoginControl::OnTime(IKernel *kernel, s64 tick)
{
	static s64 lastNotifyTick = 0;

	s32 extraCross = s_waitInfo.crossNum - s_waitInfo.rateNum;
	if (extraCross < 0)
		extraCross = 0;

	if (s_waitInfo.ququeNum < s_waitConfig.limitVal)
		s_waitInfo.rateNum = s_waitConfig.rateMax;
	else
	{
		s32 canEnterNum = s_waitConfig.limitMax - s_waitInfo.ququeNum;
		if (canEnterNum>0)
		{
			s32 nowStage = (canEnterNum / s_waitConfig.perStageNum) + 1;
			if (nowStage < 0)
				nowStage = 0;
			s_waitInfo.rateNum = nowStage * s_waitConfig.rateMax / s_waitConfig.stageNum;
			if (s_waitInfo.rateNum > canEnterNum)
				s_waitInfo.rateNum = canEnterNum;
		}
		else
			s_waitInfo.rateNum = 0;
	}
	s_waitInfo.rateNum -= extraCross;
	if (s_waitInfo.rateNum < 0)
		s_waitInfo.rateNum = 0;

	s32 lastCrossNum = s_waitInfo.crossNum;

	s_waitInfo.crossNum = 0;
	for (auto iter = s_tickIndex.begin(); (iter != s_tickIndex.end())&&(s_waitInfo.crossNum < s_waitInfo.rateNum); s_waitInfo.crossNum++)
	{
		TRACE_LOG("Account:%s, PassQueue expends:%ld ms", iter->second->accountId.c_str(), tick - iter->second->tick);
		OnPassQueue(iter->second->accountId.c_str(), iter->second->sessionId);
		s_waitAccount.erase(iter->second->accountId);
		DEL iter->second;
		iter = s_tickIndex.erase(iter);
	}

	TRACE_LOG("QueueNum:%d, Last Fram CrossNum:%d, Now RateNum:%d/S, WaitNum:%d", s_waitInfo.ququeNum, lastCrossNum, s_waitInfo.rateNum, (s32)s_tickIndex.size());
	if (tick - lastNotifyTick > s_waitConfig.updateTime)
	{
		s32 preNum = 0;
		for (const auto &tickIndex : s_tickIndex)
		{
			OnNotifyQueue(tickIndex.second, preNum);
			preNum++;
		}
		if (s_tickIndex.size() == 0)
			s_waitInfo.tickId = 0;

		lastNotifyTick = tick;
	}
}

bool LoginControl::EnterLoginQueue(const std::string &accountId, s32 sessionId)
{
	auto iter = s_waitAccount.find(accountId);
	if (iter != s_waitAccount.end())
		return false;

	if ( s_waitInfo.crossNum < s_waitInfo.rateNum)
	{
		IKernel *kernel = s_kernel;
		TRACE_LOG("Account:%s, PassQueue expends:%ld ms", accountId.c_str(), (s64)0);

		OnPassQueue(accountId.c_str(), sessionId);
		s_waitInfo.crossNum++;
	}
	else
	{
		AccountInfo *info = NEW AccountInfo();
		info->accountId = accountId;
		info->sessionId = sessionId;
		info->tickId = GetNewTickId();
		info->tick = tools::GetTimeMillisecond();

		OnEnterQueue(info, (s32)s_waitAccount.size());
		s_waitAccount.emplace(accountId, info);
		ASSERT(s_tickIndex.find(info->tickId) == s_tickIndex.end(), "error");
		s_tickIndex.emplace(info->tickId, info);
	}
	
	return true;
}

bool LoginControl::ForcePassLoginQueue(std::string &accountId)
{
	auto iter = s_waitAccount.find(accountId);
	if (iter == s_waitAccount.end())
		return false;

	OnPassQueue(accountId.c_str(), iter->second->sessionId);
	s_tickIndex.erase(iter->second->tickId);
	DEL iter->second;
	s_waitAccount.erase(iter);

	s_waitInfo.crossNum++;
	return true;
}

void LoginControl::OnPassQueue(const char *accountId, s32 sessionId)
{

}

void LoginControl::OnEnterQueue(AccountInfo *account, s32 preNum)
{
	IKernel *kernel = s_kernel;
	TRACE_LOG("Account:%s Enter Wait Queue PreNum:%d", account->accountId.c_str(), preNum);
}

void LoginControl::OnNotifyQueue(AccountInfo *account, s32 preNum)
{
	s32 waitTime = (s_waitInfo.rateNum != 0) ? ((preNum * 1000)/ s_waitInfo.rateNum) : WAIT_TIME_MAX;
	IKernel *kernel = s_kernel;
	TRACE_LOG("NotifyQueue, Account:%s, PreNum:%d, NeedWait:%d ms", account->accountId.c_str(), preNum, waitTime);
}

bool LoginControl::LoadConfigFile(IKernel *kernel)
{
	char path[MAX_PATH];
	SafeSprintf(path, sizeof(path), "%s/config.xml", kernel->GetEnvirPath());
	XmlReader reader;
	if (!reader.LoadFile(path))
	{
		ASSERT(false, "config path error");
		return false;
	}

	IXmlObject *loginConfig = reader.Root()->GetFirstChrild("loginctl");
	if (!loginConfig)
	{
		ASSERT(false, "don't find login config info");
		return false;
	}
	s_waitConfig.limitMax = loginConfig->GetAttribute_S32("limitMax");
	s_waitConfig.limitVal = loginConfig->GetAttribute_S32("limitVal");
	s_waitConfig.rateMax = loginConfig->GetAttribute_S32("rateMax");
	s_waitConfig.stageNum = loginConfig->GetAttribute_S32("stageNum");
	s_waitConfig.updateTime = loginConfig->GetAttribute_S32("updateTime");
	if (s_waitConfig.stageNum <= 0)
	{
		ASSERT(false, "stage num error");
		return false;
	}
	s_waitConfig.perStageNum = (s_waitConfig.limitMax - s_waitConfig.limitVal)/s_waitConfig.stageNum;
	if (s_waitConfig.perStageNum <= 0)
	{
		ASSERT(false, "limit val need less than limit max");
		return false;
	}
	TRACE_LOG("Wait Info:limitMax:%d, limitVal:%d, rateMax:%d, stageNum:%d, updateTime:%dms",
		s_waitConfig.limitMax, s_waitConfig.limitVal, s_waitConfig.rateMax, s_waitConfig.stageNum, s_waitConfig.updateTime);

	return true;
}
