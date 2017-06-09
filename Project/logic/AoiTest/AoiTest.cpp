/*
 * File:	AoiTest.cpp
 * Author:	xuping
 * 
 * Created On 2017/5/30 20:05:12
 */

#include "AoiTest.h"
AoiTest * AoiTest::s_self = nullptr;
IKernel * AoiTest::s_kernel = nullptr;
AoiTest::EntityMap AoiTest::s_players;

bool AoiTest::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool AoiTest::Launched(IKernel *kernel)
{
    CreateEntityMap();

    return true;
}

bool AoiTest::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

void AoiTest::CreateEntityMap()
{
    Vector3D big(100);
    Vector3D point(100);
    s32 num = 50;
    s_players.SetBoundary(big);
    s32 mark = 0;
    for (mark = 0; mark < num; mark++)
        s_players.Enter(mark, point);
    num = 302;
    Vector3D miner(10);
    s_players.SetBoundary(miner);
    for (; mark < num; mark++)
        s_players.Enter(mark, point);

    s_players.Flush();
}

