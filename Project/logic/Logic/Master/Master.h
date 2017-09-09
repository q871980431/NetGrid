/*
 * File:	Master.h
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:14:49
 */

#ifndef __Master_h__
#define __Master_h__
#include "IMaster.h"
class Master : public IMaster
{
public:
    virtual ~Master(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static Master     * s_self;
    static IKernel  * s_kernel;
};
#endif