#ifndef __IProfileMgr_h__
#define __IProfileMgr_h__
#include "MultiSys.h"
#include "ICore.h"
#include "IKernel.h"

class IProfileMgr    : public core::ICore
{
public:
	virtual void Process(s32 tick) = 0;
};
#endif
