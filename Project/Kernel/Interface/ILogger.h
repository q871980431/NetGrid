#ifndef __ILogger_h__
#define __ILogger_h__
#include "ICore.h"
#define  LOG_PREFIX_LEN         (64)

class ILogger   : public core::ICore
{
public:
    virtual ~ILogger(){};

    virtual void SyncLog(const char *contents) = 0;
    virtual void AsyncLog(const char *contents) = 0;
};

#endif