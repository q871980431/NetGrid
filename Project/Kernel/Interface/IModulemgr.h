#ifndef __IModulemgr_h__
#define __IModulemgr_h__
#include "ICore.h"
#include "IModule.h"
class IModulemgr   : public core::ICore
{
public:
    virtual ~IModulemgr(){};
    virtual IModule * FindModule(const char *name) = 0;
};

#endif