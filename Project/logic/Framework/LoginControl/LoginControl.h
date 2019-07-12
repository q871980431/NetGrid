/*
 * File:	LoginControl.h
 * Author:	xuping
 * 
 * Created On 2019/7/8 11:43:21
 */

#ifndef __LoginControl_h__
#define __LoginControl_h__
#include "ILoginControl.h"
#include <unordered_map>
#include <map>

typedef std::string AccountID;
typedef s32			SessionID;
struct AccountInfo 
{
	AccountID accountId;
	SessionID sessionId;
	u32	tickId;
	s64	tick;
};

struct  WaitConfigInfo
{
	s32 limitMax;
	s32 limitVal;
	s32 rateMax;
	s32 perStageNum;	//(limitMax - limitVal) / stageNum
	s32 stageNum;
	s32	updateTime;
};

const static s32 WAIT_TIME_MAX = 6 * 3600 * 1000;
typedef std::unordered_map<AccountID, AccountInfo*>	WaitAccountMap;
typedef std::map<u32, AccountInfo*>	TickMap;
struct WaitServiceInfo 
{
	u32 tickId;		//Æ±ºÅ
	s32 ququeNum;	
	s32 crossNum;
	s32 rateNum;
};

class LoginControl : public ILoginControl, public ITimer
{
public:
    virtual ~LoginControl(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

	virtual void OnStart(IKernel *kernel, s64 tick) {};
	virtual void OnTime(IKernel *kernel, s64 tick);
	virtual void OnTerminate(IKernel *kernel, s64 tick, bool isKill) {};

	virtual bool EnterLoginQueue(const std::string &accountId, s32 sessionId);
	virtual bool ForcePassLoginQueue(std::string &accountId);
	virtual void UpdateQueueNum(s32 num) { s_waitInfo.ququeNum = num; };

private:
	void OnPassQueue(const char *accountId, s32 sessionId);
	void OnEnterQueue(AccountInfo *account, s32 preNum);
	void OnNotifyQueue(AccountInfo *account, s32 preNum);

private:
	bool LoadConfigFile(IKernel *kernel);
	inline u32 GetNewTickId() { return ++s_waitInfo.tickId; };

private:
    static LoginControl     * s_self;
    static IKernel  * s_kernel;

	static WaitConfigInfo	s_waitConfig;
	static WaitServiceInfo	s_waitInfo;
	static WaitAccountMap	s_waitAccount;
	static TickMap			s_tickIndex;
};
#endif