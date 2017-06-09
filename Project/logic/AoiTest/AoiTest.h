/*
 * File:	AoiTest.h
 * Author:	xuping
 * 
 * Created On 2017/5/30 20:05:12
 */

#ifndef __AoiTest_h__
#define __AoiTest_h__
#include "IAoiTest.h"
#include "TRectTree.h"

class AoiTest : public IAoiTest
{
    typedef TRectTree<s32> EntityMap;

public:
    virtual ~AoiTest(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:

    static void CreateEntityMap();

private:
    static AoiTest     * s_self;
    static IKernel  * s_kernel;

    static EntityMap     s_players;
};
#endif