#include "Kernel.h"
#include "Tools.h"
#include "TDynPool.h"
#include "Zone.h"
#include "StopWatch.h"
template<  > Kernel * Singleton<Kernel>::_instance = nullptr;

bool Kernel::Ready()
{
    return _logger.Ready();
}
bool Kernel::Initialize(s32 argc, char **argv)
{
    ParseCommand(argc, argv);


    return _logger.Initialize();
}

void Kernel::Loop()
{
    IKernel *_kernel = this;
    DEBUG_LOG("xuping id = %d", 1);
    WORLD_POS lef, right;
    lef.x = 0;
    lef.z = 0;
    right.x = 10;
    right.z = 10;
    Zone<s32> zone(lef, right, 1);
    std::set<GridID_T> grids;
    for (s32 i = 0; i < 10; i++)
    {
        for (s32 j = 0; j < 10; j++)
        {
            WORLD_POS temp;
            temp.x = j;
            temp.z = i;
            s32 id = i * 10 + j;
            GridID_T grid = zone.CalcZoneID(temp);
            zone.EnterZone(id, grid);
            grids.insert(grid);
        }
    }
    static const s32  count = 110;
    s32 ids[count];
    memset(ids, 0, sizeof(ids));
    s32 num = 0;
    auto f = [&ids, &num]( const s32 &id)->bool
    {
        if (num >= count)return true;
        ids[num++] = id;
        return false;
    };
    s64 now = tools::GetTimeMillisecond();
    StopWatch watch;
    s32 count1 = 100000;
    zone.GetIDsByRadiusFromInner(54, 2, f);
    num = 0;
    memset(ids, 0, sizeof(ids));
    watch.Printf();
    //now = tools::GetTimeMillisecond();u
    zone.GetASubBIds(99, 66, 2, f);
    num = 0;
    memset(ids, 0, sizeof(ids));
    zone.GetASubBIds(66, 99, 2, f);

    system("pause");
}
void Kernel::Destroy()
{
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
