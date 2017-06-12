#ifndef __INetEngine_h__
#define __INetEngine_h__
#include "MultiSys.h"
#include "ICore.h"
#include "IKernel.h"

class INetEngine    : public core::ICore
{
public:

    virtual void Process(s32 tick) = 0;
    virtual void CreateNetSession(const char *ip, s16 port, core::IMsgSession *session) = 0;
    virtual void CreateNetListener(const char *ip, s16 port, core::ITcpListener *listener) = 0;
};
#endif
