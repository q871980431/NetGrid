/*
 * File:	Robot.cpp
 * Author:	XuPing
 * 
 * Created On 2017/9/3 22:06:53
 */

#include "Robot.h"
Robot * Robot::s_self = nullptr;
IKernel * Robot::s_kernel = nullptr;
bool Robot::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool Robot::Launched(IKernel *kernel)
{

    return true;
}

bool Robot::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


