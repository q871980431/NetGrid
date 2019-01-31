/*
 * File:	IpcShareMemory.h
 * Author:	xuping
 * 
 * Created On 2018/12/4 20:37:05
 */

#ifndef __IpcShareMemory_h__
#define __IpcShareMemory_h__
#include "IIpcShareMemory.h"
class IpcShareMemory : public IIpcShareMemory
{
public:
    virtual ~IpcShareMemory(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

	virtual char * CreateShareBuff(s64 &id, s32 buffSize, const char *traceInfo);
	virtual char * OpenShareBuff(s64 id, s32 &buffSize, const char *traceInfo);
	virtual void   DestroyShareBuff(s64 id);
	virtual void   CloseShareBuff(s64 id);
protected:
private:
    static IpcShareMemory     * s_self;
    static IKernel  * s_kernel;
};

#endif