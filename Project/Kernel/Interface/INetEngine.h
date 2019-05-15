#ifndef __INetEngine_h__
#define __INetEngine_h__
#include "MultiSys.h"
#include "ICore.h"
#include "IKernel.h"

class INetEngine    : public core::ICore
{
public:

    virtual void Process(core::IKernel *kernel, s32 tick) = 0;
    virtual void CreateNetSession(const char *ip, s32 port, core::ITcpSession *session) = 0;
    virtual void CreateNetListener(const char *ip, s32 port, core::INetTcpListener *listener) = 0;
};
#endif
