#include "Test.h"
#include "TDynPool.h"
#include "Tools_time.h"
#include "hmac.h"
#include <map>
Test * Test::s_self = nullptr;
IKernel * Test::s_kernel = nullptr;

void MyTimer::OnStart(IKernel *kernel, s64 tick)
{
	ECHO("Call Start, time = %dms", (tick - _tick));
}

void MyTimer::OnTime(IKernel *kernel, s64 tick)
{
	//ECHO("Call Time, time = %dms", (tick - _tick));
    _tick = tick;
}

void MyTimer::OnTerminate(IKernel *kernel, s64 tick)
{
	ECHO("Call Terminate, time = %dms", tick - _tick);
	kernel->StartTimer(this, 274, 10, 995, "on Terminate");
}
bool Test::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    char buff[] = "xuping";
    buff[2] = 0;
    std::string name = "sadgsagas";
    name[3] = '\0';
    s32 size = name.size();
    ECHO("%s", name.c_str());
    
    return true;
}

bool Test::Launched(IKernel *kernel)
{
    //TestLinkList();
    TestBitMark();
	s64 now = tools::GetTimeMillisecond();
    MyTimer *timer = NEW MyTimer(1, now);
    s_kernel->StartTimer(timer, 3000, FOREVER, 1000, "my timer");

    return true;
}

bool Test::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

void Test::TestLinkList()
{
    tlib::TDynPool<Player> playerPool;
    tlib::linear::LinkList players;
    tlib::linear::LinkList player1;
    Player *tmp1 = nullptr;

    for (s32 i = 0; i < 10; i++)
    {
        Player *tmp = CREAT_FROM_POOL(playerPool, i);
        if (i % 2 == 0)
            players.HeadInsert(tmp);
        else
            players.TailInsert(tmp);
        if (i == 4)
            tmp1 = tmp;
    }
    for (s32 i = 0; i < 5; i++)
    {
        players.TailRemove();
    }
    players.Remove(tmp1);
    for (s32 i = 0; i < 10; i++)
    {
        Player *tmp = CREAT_FROM_POOL(playerPool, i);
        player1.TailInsert(tmp);
    }

    tlib::linear::ILinkNode *node = players.Head();
    while (node)
    {
        Player *tmp = (Player *)node;
        tmp->Printf();
        node = node->_next;
    }
    node = player1.Head();
    while (node)
    {
        Player *tmp = (Player *)node;
        //tmp->Printf();
        node = node->_next;
    }
}

void Test::TestBitMark()
{
    BitMark mark;
    mark.SetMark(5);
    mark.SetMark(6);
    bool marked = mark.Marked(5);
    bool marke1 = mark.Marked(8);
    mark.CancelMark(5);
    bool marke2 = mark.Marked(5);
    bool marke3 = mark.Marked(6);
    
}
inline u8 ToHexL(u8 x){ return x > 9 ? x + 87 : x + 48; }
inline u8 ToHexH(u8 x){ return x > 9 ? x + 55 : x + 48; }
void Test::CreateStr(char *buff, s32 len, std::string &str)
{
    for (s32 i = 0; i < len; i++)
    {
        str += ToHexL((u8)buff[i] >> 4);
        str += ToHexL((u8)buff[i] % 16);
    }
}

void Test::TestHmacSha1()
{
    char buff[MAX_PATH] = { 0 };
    const char *key = "123456";
    const char *data = "testest";
    size_t size = 256;
    hmac_sha1((const unsigned char *)key, strlen(key), (const unsigned char *)data, strlen(data), (unsigned char *)buff, &size);
    std::string tmp("");
    CreateStr(buff, size, tmp);
    ECHO("%s,%d", tmp.c_str(), tmp.length());
}