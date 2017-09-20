/*
 * File:	Slave.h
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:15:12
 */

#ifndef __Slave_h__
#define __Slave_h__
#include "ISlave.h"
#include "TString.h"
struct MachineInfo 
{
	s32 cpuNum;
	s32 processId;

	s64 cpufreshTick;
	s64 collectTick;
	s64 expensTick;
	s32 idlePercent;
};

struct PortConfig 
{
	s32 innerPortBegin;
	s32 innerPortEnd;
	s32 outterPortBegin;
	s32 outterPortEnd;
};

class Slave : public ISlave
{
	typedef tlib::TString<MAX_PATH> FilePath;
public:
    virtual ~Slave(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
private:
	static s32 CreateProcess(const char *cmd);
	static void InitMachineInfo();
protected:
private:
    static Slave     * s_self;
    static IKernel  * s_kernel;

	static MachineInfo	s_machineInfo;
	static FilePath		s_runDir;
};

class ProcessPercentTimer	: public ITimer
{
public:
	ProcessPercentTimer(MachineInfo *machineInfo) :_machineInfo(machineInfo) {};
	virtual ~ProcessPercentTimer() {};

	virtual void OnStart(IKernel *kernel, s64 tick) {};
	virtual void OnTime(IKernel *kernel, s64 tick);
	virtual void OnTerminate(IKernel *kernel, s64 tick) { DEL this; };
protected:
private:
	MachineInfo	*_machineInfo;
};


class CpuIdlePercentTimer : public ITimer
{
public:
	CpuIdlePercentTimer(MachineInfo *machineInfo):_machineInfo(machineInfo), _preTotalTick(0), _preIdleTick(0), _freshTick(0){};
	virtual ~CpuIdlePercentTimer(){};

	virtual void OnStart(IKernel *kernel, s64 tick) {};
	virtual void OnTime(IKernel *kernel, s64 tick);
	virtual void OnTerminate(IKernel *kernel, s64 tick) { DEL this; };
protected:
private:
	MachineInfo	*_machineInfo;
	s64 _preTotalTick;

	s64 _preIdleTick;
	s64 _freshTick;
};

#endif