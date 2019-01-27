/*
 * File:	IIpcShareMemory.h
 * Author:	xuping
 * 
 * Created On 2018/12/4 20:37:05
 */

#ifndef  __IIpcShareMemory_h__
#define __IIpcShareMemory_h__
#include "IModule.h"

class IIpcShareMemory : public IModule
{
public:
	enum AccessType
	{
		WRITE = 0,
		READ = 1,
	};

    virtual ~IIpcShareMemory(){};
	virtual char * CreateShareBuff(s64 &id, s32 buffSize, const char *traceInfo) =0;
	virtual char * OpenShareBuff(s64 id, s32 &buffSize, const char *traceInfo) = 0;
	virtual void   DestroyShareBuff(s64 id) = 0;
	virtual void   CloseShareBuff(s64 id) = 0;
};
#endif