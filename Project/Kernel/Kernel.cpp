#include "Kernel.h"
#include "Tools.h"
#include "TDynPool.h"
#include "StopWatch.h"
#include "Configmgr.h"
#include "Modulemgr.h"
#include "NetService.h"
#include "TimerMgr.h"
#include "FrameMgr.h"
#include "AsyncQueue.h"
#include "XmlReader.h"
#include "ExceptionMgr.h"
#include "ProfileMgr.h"

template<> Kernel * Singleton<Kernel>::_instance = nullptr;
core::IKernel * G_KERNEL::g_kernel = nullptr;
s32 G_KERNEL::g_logLvl = 0;

const char ProcessNameSplit = '_';
const s32 FRAME_NUM_UNLIMIT = -1;
const s32 MODULE_EXPENDS_TIME = 10;
const s32 KERNEL_SLEEP_TIME = 10;
const s32 KERNEL_LOAD_PRF_TIME = 1000;

bool Kernel::Ready()
{
	_asyncQueueId = 1;
	_mainQueue = nullptr;
	_frameNum = FRAME_NUM_UNLIMIT;
	G_KERNEL::g_kernel = this;
	G_KERNEL::g_logLvl = LOG_LEVEL_DEBUG;
    return _logger.Ready()&&
           Configmgr::GetInstance()->Ready()&&
           NetService::GetInstance()->Ready()&&
		   TimerMgr::GetInstance()->Ready() && PROFILEMGR.Ready();
}
bool Kernel::Initialize(s32 argc, char **argv)
{
    ParseCommand(argc, argv);
	const char *name = GetCmdArg("name");
	const char *id = GetCmdArg("id");
	if (name == nullptr || id == nullptr)
	{
		ASSERT(false, "cmd args need have name and id");
		return false;
	}
	const char *framNum = GetCmdArg("frame");
	if (framNum != nullptr)
	{
		int32_t iNum = std::atoi(framNum);
		if (iNum > 0)
		{
			_frameNum = iNum;
		}
	}
	_procName.append(name);
	_procName.push_back(ProcessNameSplit);
	_procName.append(id);

	bool temp = _logger.Initialize() && Configmgr::GetInstance()->Initialize();
	if (temp)
	{
		const char *configPath = GetConfigFile();
		XmlReader reader;
		if (!reader.LoadFile(configPath))
		{
			ASSERT(false, "don't load config, file = %s", configPath);
			return false;
		}
		IXmlObject *async = reader.Root()->GetFirstChrild("async");
		if (async && async->HasAttribute("threadcount"))
		{
			s32 threadCount = async->GetAttribute_S32("threadcount");
			if (threadCount > 0)
			{
				_mainQueue =  CreateAsyncQueue(0, threadCount, "config.xml");
				if (_mainQueue == nullptr)
					return false;
			}
		}

		return         NetService::GetInstance()->Initialize()
			&& TimerMgr::GetInstance()->Initialize()
			&& Modulemgr::GetInstance()->Initialize() && PROFILEMGR.Initialize();
	}

	return temp;
}

#define TRACE_LOG_STOP_WATCH(stopWatch, Time, Content)\
{\
	int64_t costTime = stopWatch.Interval();\
	if (costTime > (Time))\
		TRACE_LOG(Content expends time:%ld, costTime);\
	stopWatch.Reset();\
}

void Kernel::Loop()
{
	ExceptionMgr::Init();
	s64 iLastTime = tools::GetTimeMillisecond();
	s64 iNowTime = iLastTime;
	s32	iCpuTime = 0;
	tools::StopWatch<> stopWatch;
	tools::StopWatch<> loadWatch;
	tools::StopWatch<> cpuWatch;
    while (true)
    {
		iNowTime = tools::GetTimeMillisecond();
		stopWatch.Reset();
		cpuWatch.Reset();
		if (!_asyncQueues.empty())
		{
			s32 execTime = MODULE_EXPENDS_TIME / _asyncQueues.size();
			if (execTime == 0)
				execTime = 1;

			for (auto &asyncQueue : _asyncQueues)
				asyncQueue.second->Loop(execTime);
		}
		TRACE_LOG_STOP_WATCH(stopWatch, 0, async queue);
		NetService::GetInstance()->Process(this, MODULE_EXPENDS_TIME);
		TRACE_LOG_STOP_WATCH(stopWatch, MODULE_EXPENDS_TIME, "net service");
		TimerMgr::GetInstance()->Process(MODULE_EXPENDS_TIME);
		TRACE_LOG_STOP_WATCH(stopWatch, MODULE_EXPENDS_TIME, "timer manager");
		FrameMgr::GetInstance()->Process(MODULE_EXPENDS_TIME);
		TRACE_LOG_STOP_WATCH(stopWatch, MODULE_EXPENDS_TIME, "frame manager");
		PROFILEMGR.Process(MODULE_EXPENDS_TIME);
		TRACE_LOG_STOP_WATCH(stopWatch, MODULE_EXPENDS_TIME, "profile manager");
		_logger.Process(MODULE_EXPENDS_TIME);
		TRACE_LOG_STOP_WATCH(stopWatch, MODULE_EXPENDS_TIME, "log manager");
		iCpuTime += cpuWatch.Interval();
		MSLEEP(KERNEL_SLEEP_TIME);
		s64 interval = loadWatch.Interval();
		if (loadWatch.Interval() > KERNEL_LOAD_PRF_TIME)
		{
			TRACE_LOG("runtime:%ld ms, cpu time:%ld ms, load val:%.2f%%", interval, iCpuTime, (iCpuTime * 100.f) / interval);
			loadWatch.Reset();
			iCpuTime = 0;
		}
    }
}
void Kernel::Destroy()
{
	TimerMgr::GetInstance()->Destroy();
	NetService::GetInstance()->Destroy();
    Modulemgr::GetInstance()->Destroy();
    Configmgr::GetInstance()->Destroy();
    _logger.Destroy(); 
}
const char * Kernel::GetCmdArg(const char *name)
{
    auto iter = _cmdArgs.find(name);
    return iter == _cmdArgs.end() ? nullptr : iter->second.c_str();
}

void Kernel::ParseCommand(s32 argc, char **argv)
{
    for (s32 i = 1; i < argc; i++)
    {
        char *start = argv[i] + 0;
        if (*start != '-')
        {
            ASSERT(false, "argv[%i]=[%s], illegal", i, argv[i]);
            continue;
        }
        char *equal = strstr(start, "=");
        if (nullptr == equal)
        {
            ASSERT(false, "argv[%i]=[%s], illegal", i, argv[i]);
            continue;
        }
        std::string name(start + 2, equal);
        std::string value(equal + 1);
        if (!_cmdArgs.insert(std::make_pair(name, value)).second)
        {
            ASSERT(false, "arg[%d], name = %s has exist", i, name.c_str());
        }
    }
}
void Kernel::AsyncLog(const char *contens)
{
    _logger.AsyncLog(contens);
}
void Kernel::SyncLog(const char *contens)
{
    _logger.SyncLog(contens);
}

void Kernel::ThreadLog(const char *contents)
{
	_logger.ThreadLog(contents);
}

s32 Kernel::GetLogLevel()
{
	return _logger.LogLevel();
}

IModule * Kernel::FindModule(const char *name)
{
    ASSERT(name != nullptr, "errpr");
    return Modulemgr::GetInstance()->FindModule(name);
}

void Kernel::CreateNetListener(const char *ip, s16 port, core::INetTcpListener *listener)
{
    NetService::GetInstance()->CreateNetListener(ip, port, listener);
}

void Kernel::CreateNetSession(const char *ip, s16 port, core::ITcpSession *session)
{
    NetService::GetInstance()->CreateNetSession(ip, port, session);
}

void Kernel::StartTimer(core::ITimer *timer, s64 delay, s32 count, s64 interval, const char *trace)
{
	TimerMgr::GetInstance()->StartTimer(timer, delay, count, interval, trace);
}

void Kernel::KillTimer(core::ITimer *timer)
{
	TimerMgr::GetInstance()->KillTimer(timer);
}

void Kernel::AddFrame(core::IFrame *frame, u8 runLvl, const char *trace)
{
	FrameMgr::GetInstance()->AddFrame(frame, runLvl, trace);
}

void Kernel::RemoveFrame(core::IFrame *frame)
{
	FrameMgr::GetInstance()->DelFrame(frame);
}

void Kernel::StartAsync(const s64 threadId, core::IAsyncHandler * handler, const char * file, const s32 line)
{
	if (_mainQueue )
		_mainQueue->StartAsync(threadId, handler, file, line);
}

void Kernel::StopAsync(core::IAsyncHandler * handler)
{
	if (_mainQueue)
		_mainQueue->StopAsync(handler);
}

core::IAsyncQueue * Kernel::GetMainAsyncQueue()
{
	return _mainQueue;
}

core::IAsyncQueue * Kernel::CreateAsyncQueue(const s32 threadSize, const char *trace)
{
	return CreateAsyncQueue(GetNewQueueId(), threadSize, trace);
}

AsyncQueue * Kernel::CreateAsyncQueue(s32 queueId, const s32 threadSize, const char *trace)
{
	IKernel *kernel = this;

	AsyncQueue *queue = NEW AsyncQueue(queueId, trace);
	if (!queue->Initialize(threadSize))
	{
		DEL queue;
		return nullptr;
	}
	TRACE_LOG("Create Async Queue sucess, Id:%d, name:%s, threadcount:%d", queueId, trace, threadSize);
	_asyncQueues.emplace(queueId, queue);
	return queue;
}

const char * Kernel::GetCoreFile()
{
    return Configmgr::GetInstance()->GetCoreFile();
}

const char * Kernel::GetConfigFile()
{
    return Configmgr::GetInstance()->GetConfigFile();
}

const char * Kernel::GetEnvirPath()
{

    return Configmgr::GetInstance()->GetEnvirPath();
}