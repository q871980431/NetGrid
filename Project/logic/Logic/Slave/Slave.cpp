/*
 * File:	Slave.cpp
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:15:12
 */

#include "Slave.h"
#include <ShlObj.h>
#include <shellapi.h>
#include "Tools.h"
#include "Tools_time.h"
#ifdef WIN32
#include <process.h>
#endif

Slave * Slave::s_self = nullptr;
IKernel * Slave::s_kernel = nullptr;
MachineInfo Slave::s_machineInfo;
Slave::FilePath Slave::s_runDir;

bool Slave::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	tools::SafeMemset(&s_machineInfo, sizeof(s_machineInfo), 0, sizeof(s_machineInfo));
	s_runDir << tools::GetAppPath() << "/run/";
	InitMachineInfo();
	CpuIdlePercentTimer *timer = NEW CpuIdlePercentTimer(&s_machineInfo);
	kernel->StartTimer(timer, 1000, FOREVER, 1000, "ProcessPercentTimer");
	char path[MAX_PATH] = { 0 };
	char path1[MAX_PATH] = { 0 };
	SafeSprintf(path, sizeof(path), "%s,%d", "xuping", 1);
	SafeSprintf(path1, sizeof(path1), "%s,", "xuping", 1);

	ECHO("path = %s", path);
	ECHO("path = %s", path1);
    return true;
}

bool Slave::Launched(IKernel *kernel)
{
    return true;
}

bool Slave::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

s32 Slave::CreateProcess(const char *cmd)
{
	STARTUPINFO si; //一些必备参数设置  
	memset(&si, 0, sizeof(STARTUPINFO));
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = TRUE; //TRUE表示显示创建的进程的窗口
	PROCESS_INFORMATION pi;
	char title[MAX_PATH];
	char cmdline[MAX_PATH];
	SafeSprintf(cmdline, sizeof(cmdline), "%s", cmd);
	SafeSprintf(title, sizeof(title), "%s", cmd);
	si.lpTitle = title;

	BOOL bRet = ::CreateProcess(
		nullptr,
		cmdline, //在Unicode版本中此参数不能为常量字符串，因为此参数会被修改    
		NULL,
		NULL,
		FALSE,
		CREATE_NEW_CONSOLE,
		NULL,
		NULL,
		&si,
		&pi);

	int error = GetLastError();
	if (bRet)
	{
		::CloseHandle(pi.hThread);
		::CloseHandle(pi.hProcess);

		printf(" 新进程的进程ID号：%d /n", pi.dwProcessId);
		printf(" 新进程的主线程ID号：%d /n", pi.dwThreadId);
	}
	else
	{
		printf("error code:%d/n", error);
	}

	return pi.dwProcessId;
}

void Slave::InitMachineInfo()
{
#ifdef WIN32
	SYSTEM_INFO sysInfo = { 0 };
	GetSystemInfo(&sysInfo);
	s_machineInfo.cpuNum = sysInfo.dwNumberOfProcessors;
	s_machineInfo.processId = getpid();
#endif // WIN32
}

void ProcessPercentTimer::OnTime(IKernel *kernel, s64 tick)
{
	FILETIME CreateTime, ExitTime, KernelTime, UserTime;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, _machineInfo->processId);
	BOOL bRetCode = GetProcessTimes(hProcess, &CreateTime, &ExitTime, &KernelTime, &UserTime);
	s32 percent = -1;
	if (bRetCode)
	{
		LARGE_INTEGER lgKernelTime;
		LARGE_INTEGER lgUserTime;
		LARGE_INTEGER lgCurTime;

		lgKernelTime.HighPart = KernelTime.dwHighDateTime;
		lgKernelTime.LowPart = KernelTime.dwLowDateTime;
		lgUserTime.HighPart = UserTime.dwHighDateTime;
		lgUserTime.LowPart = UserTime.dwLowDateTime;

		s64 expsTick = (lgKernelTime.QuadPart + lgUserTime.QuadPart) / 10000;
		if (_machineInfo->expensTick == 0)
		{
			_machineInfo->expensTick = expsTick;
			_machineInfo->cpufreshTick = tick;
		}
		else
		{
			percent = ((expsTick - _machineInfo->expensTick) * 100) / ((tick - _machineInfo->cpufreshTick) * _machineInfo->cpuNum);
		}
	}
	DEBUG_LOG("percent = %d, core = %d", percent, _machineInfo->cpuNum);
}

void CpuIdlePercentTimer::OnTime(IKernel *kernel, s64 tick)
{
	FILETIME idleTime, kernelTime, userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);
	LARGE_INTEGER lgKernelTime;
	LARGE_INTEGER lgUserTime;
	LARGE_INTEGER lgIdleTime;
	lgKernelTime.HighPart = kernelTime.dwHighDateTime;
	lgKernelTime.LowPart = kernelTime.dwLowDateTime;
	lgUserTime.HighPart = userTime.dwHighDateTime;
	lgUserTime.LowPart = userTime.dwLowDateTime;
	lgIdleTime.HighPart = idleTime.dwHighDateTime;
	lgIdleTime.LowPart = idleTime.dwLowDateTime;

	if (_freshTick != 0)
	{
		_machineInfo->idlePercent = (lgIdleTime.QuadPart - _preIdleTick) * 100 / (lgKernelTime.QuadPart + lgUserTime.QuadPart - _preTotalTick);
	}
	_freshTick = tick;
	_preTotalTick = lgKernelTime.QuadPart + lgUserTime.QuadPart;
	_preIdleTick = lgIdleTime.QuadPart;
	DEBUG_LOG("machine idle percent = %d, core = %d", _machineInfo->idlePercent, _machineInfo->cpuNum);
}