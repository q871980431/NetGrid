/*
 * File:	IpcShareMemory.cpp
 * Author:	xuping
 * 
 * Created On 2018/12/4 20:37:05
 */

#include "IpcShareMemory.h"
IpcShareMemory * IpcShareMemory::s_self = nullptr;
IKernel * IpcShareMemory::s_kernel = nullptr;
bool IpcShareMemory::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool IpcShareMemory::Launched(IKernel *kernel)
{

    return true;
}

bool IpcShareMemory::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

char * IpcShareMemory::CreateShareBuff(s64 &id, s32 buffSize, const char *traceInfo)
{
	return nullptr;
}

char * IpcShareMemory::OpenShareBuff(s64 id, s32 &buffSize, const char *traceInfo)
{
	return nullptr;
}

void IpcShareMemory::DestroyShareBuff(s64 id)
{

}

void IpcShareMemory::CloseShareBuff(s64 id)
{

}
