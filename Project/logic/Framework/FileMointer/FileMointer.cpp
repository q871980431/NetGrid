/*
 * File:	FileMointer.cpp
 * Author:	XuPing
 * 
 * Created On 2019/6/3 15:50:05
 */

#include "FileMointer.h"
FileMointer * FileMointer::s_self = nullptr;
IKernel * FileMointer::s_kernel = nullptr;
bool FileMointer::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool FileMointer::Launched(IKernel *kernel)
{

    return true;
}

bool FileMointer::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


