/*
 * File:	RankMgr.cpp
 * Author:	XuPing
 * 
 * Created On 2019/5/9 17:23:15
 */

#include "RankMgr.h"
RankMgr * RankMgr::s_self = nullptr;
IKernel * RankMgr::s_kernel = nullptr;
bool RankMgr::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool RankMgr::Launched(IKernel *kernel)
{

    return true;
}

bool RankMgr::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


