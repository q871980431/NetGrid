/*
 * File:	ILoginControl.h
 * Author:	xuping
 * 
 * Created On 2019/7/8 11:43:21
 */

#ifndef  __ILoginControl_h__
#define __ILoginControl_h__
#include "IModule.h"
class ILoginControl : public IModule
{
public:
    virtual ~ILoginControl(){};
    
	virtual bool EnterLoginQueue(const std::string &accountId, s32 sessionId) = 0;
	virtual void UpdateQueueNum(s32 num) = 0;
};
#endif