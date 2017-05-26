#include "Kernel.h"
#include "Tools.h"
#include "TDynPool.h"
#include "StopWatch.h"
#include "Configmgr.h"
#include "Modulemgr.h"
template<  > Kernel * Singleton<Kernel>::_instance = nullptr;

bool Kernel::Ready()
{
    return _logger.Ready()&&
           Configmgr::GetInstance()->Ready();
}
bool Kernel::Initialize(s32 argc, char **argv)
{
    ParseCommand(argc, argv);

    return _logger.Initialize() 
        && Configmgr::GetInstance()->Initialize()
        && Modulemgr::GetInstance()->Initialize();
}

void Kernel::Loop()
{
    system("pause");
}
void Kernel::Destroy()
{
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
        std::string name(start + 1, equal);
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