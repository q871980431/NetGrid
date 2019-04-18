/*
 * File:	AI.h
 * Author:	xuping
 * 
 * Created On 2019/3/22 11:59:34
 */

#ifndef __AI_h__
#define __AI_h__
#include "IAI.h"
class AI : public IAI
{
public:
    virtual ~AI(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static AI     * s_self;
    static IKernel  * s_kernel;
};
#endif