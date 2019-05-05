#ifndef __IFrameMgr_h__
#define __IFrameMgr_h__
#include "MultiSys.h"
#include "ICore.h"
#include "IKernel.h"

class IFrameMgr    : public core::ICore
{
public:
    virtual void Process(s32 tick) = 0;
    virtual void AddFrame(core::IFrame *framer, u8 runLvl, const char *debug) = 0;
    virtual void DelFrame(core::IFrame *framer) = 0;
};
#endif
