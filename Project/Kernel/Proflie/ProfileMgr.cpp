#include "ProfileMgr.h"
#include "XmlReader.h"
#include "../Kernel.h"
#include "Tools_time.h"
#include "gperftools/profiler.h"
#include "gperftools/heap-profiler.h"
#include "gperftools/malloc_extension.h"

bool ProfileMgr::Ready()
{
	tools::Zero(_config);
	return true;
}

bool ProfileMgr::Initialize()
{
	_LoadConfig();
#ifdef LINUX
	if (!_LoadConfig())
		return false;

	const char *procName = Kernel::GetInstance().GetProcName();
	if (_config.openCpuInfo)
	{
		std::string	cpuProfFile(procName);
		cpuProfFile.append(".cpuprof");
		ProfilerStart(cpuProfFile.c_str());
	}
	if (_config.openMemInfo)
	{
		std::string	heapProfPrefix(procName);
		heapProfPrefix.append("heap");
		HeapProfilerStart(heapProfPrefix.c_str());
	}
#endif // LINUX
	return true;
}

bool ProfileMgr::Destroy()
{
#ifdef LINUX
	if (_config.openCpuInfo)
	{
		ProfilerStop();
	}
	if (_config.openMemInfo)
	{
		HeapProfilerStop();
	}
#endif
	return true;
}

void ProfileMgr::Process(s32 tick)
{
#ifdef LINUX
	s64 now = tools::GetTimeMillisecond();
	if (_config.lastCheckTime == 0)
	{
		_config.lastCheckTime = now;
	}
	if (now - _config.lastCheckTime > _config.checkTime)
	{
		ECHO("************Enter Profiler Flush************");
		if (_config.openCpuInfo)
		{
			ProfilerFlush();
		}
		if (_config.openMemInfo)
		{
			HeapProfilerDump("loop dump");
		}
		_config.lastCheckTime = now;
	}
#endif
}

bool ProfileMgr::_LoadConfig()
{
	tools::Zero(_config);
	const char *configPath = Kernel::GetInstance().GetConfigFile();
	XmlReader reader;
	if (!reader.LoadFile(configPath))
	{
		ASSERT(false, "don't load config, file = %s", configPath);
		return false;
	}
	IXmlObject *profile = reader.Root()->GetFirstChrild("profile");
	if (profile != nullptr)
	{
		_config.openCpuInfo = profile->GetAttribute_Bool("cpu_check");
		_config.openMemInfo = profile->GetAttribute_Bool("mem_check");
		_config.checkTime = profile->GetAttribute_S32("check_time") * tools::MILLISECONDS;
	}

	return true;
}



