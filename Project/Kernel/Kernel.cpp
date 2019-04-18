#include "Kernel.h"
#include "Tools.h"
#include "TDynPool.h"
#include "StopWatch.h"
#include "Configmgr.h"
#include "Modulemgr.h"
#include "NetEngine.h"
#include "TimerMgr.h"

template<> Kernel * Singleton<Kernel>::_instance = nullptr;

bool Kernel::Ready()
{
    return _logger.Ready()&&
           Configmgr::GetInstance()->Ready()&&
           NetEngine::GetInstance()->Ready()&&
		   TimerMgr::GetInstance()->Ready();
}
bool Kernel::Initialize(s32 argc, char **argv)
{
    ParseCommand(argc, argv);

    return _logger.Initialize() 
        && Configmgr::GetInstance()->Initialize()
        && Modulemgr::GetInstance()->Initialize()
        && NetEngine::GetInstance()->Initialize()
		&& TimerMgr::GetInstance()->Initialize();
}

void Kernel::Loop()
{
    while (true)
    {
        NetEngine::GetInstance()->Process(10);
		TimerMgr::GetInstance()->Process(10);
		_logger.Process(10);
		MSLEEP(10);
    }
}
void Kernel::Destroy()
{
	TimerMgr::GetInstance()->Destroy();
	NetEngine::GetInstance()->Destroy();
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

IModule * Kernel::FindModule(const char *name)
{
    ASSERT(name != nullptr, "errpr");
    return Modulemgr::GetInstance()->FindModule(name);
}

void Kernel::CreateNetListener(const char *ip, s16 port, core::ITcpListener *listener)
{
    NetEngine::GetInstance()->CreateNetListener(ip, port, listener);
}

void Kernel::CreateNetSession(const char *ip, s16 port, core::IMsgSession *session)
{
    NetEngine::GetInstance()->CreateNetSession(ip, port, session);
}

void Kernel::StartTimer(core::ITimer *timer, s32 delay, s32 count, s32 interval, const char *trace)
{
	TimerMgr::GetInstance()->StartTimer(timer, delay, count, interval, trace);
}

void Kernel::KillTimer(core::ITimer *timer)
{
	TimerMgr::GetInstance()->KillTimer(timer);
}

void Kernel::AddFrame(core::IFrame *frame, u8 runLvl)
{

}

void Kernel::RemoveFrame(core::IFrame *frame)
{

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