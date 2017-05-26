#ifndef __INetEngine_h__
#define __INetEngine_h__
#include "MultiSys.h"
#include "ICore.h"

class INetEngine    : public core::ICore
{
public:

    virtual void Process(s32 tick) = 0;
};
#endif
