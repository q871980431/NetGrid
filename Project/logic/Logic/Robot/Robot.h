/*
 * File:	Robot.h
 * Author:	XuPing
 * 
 * Created On 2017/9/3 22:06:53
 */

#ifndef __Robot_h__
#define __Robot_h__
#include "IRobot.h"
class Robot : public IRobot
{
public:
    virtual ~Robot(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static Robot     * s_self;
    static IKernel  * s_kernel;
};
#endif