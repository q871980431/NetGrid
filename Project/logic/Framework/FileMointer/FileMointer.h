/*
 * File:	FileMointer.h
 * Author:	XuPing
 * 
 * Created On 2019/6/3 15:50:05
 */

#ifndef __FileMointer_h__
#define __FileMointer_h__
#include "IFileMointer.h"
class FileMointer : public IFileMointer
{
public:
    virtual ~FileMointer(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static FileMointer     * s_self;
    static IKernel  * s_kernel;
};
#endif