#include "Test.h"
Test * Test::s_self = nullptr;
IKernel * Test::s_kernel = nullptr;
bool Test::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool Test::Launched(IKernel *kernel)
{
    ATTR attr;
    s32 id = attr("xuping");
    s16 val = attr.GetDataT<s16>(16);
    bool vb = attr.GetDataT<bool>(16);
    s32 tmp = 16;
    float vf = attr.GetDataT<float>(tmp);
    float vf2 = *reinterpret_cast<float *>(&tmp);
    char buff[8];
    memcpy(buff, &vf2, sizeof(vf2));
    buff[sizeof(vf2)] = 0;
    float vf1 = (float)tmp;
    memcpy(buff, &vf1, sizeof(vf1));
    buff[sizeof(vf1)] = 0;

    return true;
}

bool Test::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}