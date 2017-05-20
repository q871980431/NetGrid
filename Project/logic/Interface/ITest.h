#ifndef  __ITest_h__
#define __ITest_h__
#include "IModule.h"
class ITest : public IModule
{
public:
    virtual ~ITest(){};
    
    virtual s32 Add(s32 a, s32 b) = 0;
};
#endif