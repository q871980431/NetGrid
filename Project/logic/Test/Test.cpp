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

    return true;
}

bool Test::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}