#include "Test.h"
#include "TDynPool.h"
#include "Tools_time.h"
Test * Test::s_self = nullptr;
IKernel * Test::s_kernel = nullptr;

void MyTimer::OnStart(IKernel *kernel, s64 tick)
{
	ECHO("Call Start, time = %dms", (tick - _tick));
}

void MyTimer::OnTime(IKernel *kernel, s64 tick)
{
	ECHO("Call Time, time = %dms", (tick - _tick));
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
    
    return true;
}

bool Test::Launched(IKernel *kernel)
{
    //TestLinkList();
    TestBitMark();
	s64 now = tools::GetTimeMillisecond();
	//MyTimer *timer = NEW MyTimer(1, now);
	//s_kernel->StartTimer(timer, 3000, FOREVER, 2000, "my timer");
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