/*
 * File:	Slave.h
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:15:12
 */

#ifndef __Slave_h__
#define __Slave_h__
#include "ISlave.h"
class Slave : public ISlave
{
public:
    virtual ~Slave(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static Slave     * s_self;
    static IKernel  * s_kernel;
};
#endif