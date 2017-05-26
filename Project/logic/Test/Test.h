#ifndef __Test_h__
#define __Test_h__
#include "ITest.h"
class Test : public ITest
{
public:
    virtual ~Test(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
    virtual s32 Add(s32 a, s32 b){ return a + b; };
protected:
private:
    static Test     * s_self;
    static IKernel  * s_kernel;
};
#endif