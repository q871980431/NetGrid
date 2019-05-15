/*
 * File:	RankMgr.h
 * Author:	XuPing
 * 
 * Created On 2019/5/9 17:23:15
 */

#ifndef __RankMgr_h__
#define __RankMgr_h__
#include "IRankMgr.h"
class RankMgr : public IRankMgr
{
public:
    virtual ~RankMgr(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static RankMgr     * s_self;
    static IKernel  * s_kernel;
};
#endif