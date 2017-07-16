#include "Test.h"
#include "TDynPool.h"
Test * Test::s_self = nullptr;
IKernel * Test::s_kernel = nullptr;
bool Test::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    
    return true;
}

bool Test::Launched(IKernel *kernel)
{
    TestLinkList();
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