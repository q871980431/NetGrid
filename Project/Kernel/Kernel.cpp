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

template<> Kernel * Singleton<Kernel>::_instance = nullptr;
core::IKernel * G_KERNEL::g_kernel = nullptr;

bool Kernel::Ready()
{
	_asyncQueueId = 1;
	_mainQueue = nullptr;
	G_KERNEL::g_kernel = this;
    return _logger.Ready()&&
           Configmgr::GetInstance()->Ready()&&
           NetService::GetInstance()->Ready()&&
		   TimerMgr::GetInstance()->Ready();
}
bool Kernel::Initialize(s32 argc, char **argv)
{
    ParseCommand(argc, argv);


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
			&& Modulemgr::GetInstance()->Initialize();
	}

	return temp;
}

void Kernel::Loop()
{
	ExceptionMgr::Init();
    while (true)
    {
		if (!_asyncQueues.empty())
		{
			s32 execTime = 10 / _asyncQueues.size();
			if (execTime == 0)
				execTime = 1;

			for (auto &asyncQueue : _asyncQueues)
				asyncQueue.second->Loop(execTime);
		}

		NetService::GetInstance()->Process(this,10);
		TimerMgr::GetInstance()->Process(10);
		FrameMgr::GetInstance()->Process(10);
		_logger.Process(10);
		MSLEEP(5);
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