#ifndef __Test_h__
#define __Test_h__
#include "ITest.h"
#include "Tools.h"
#include "TList.h"

struct ATTR 
{
    s32 operator ()(const char *name)
    {
        static s32 id = 0;
        if (id == 0)
            id = tools::HashKey(name);
        return id;
    }
    template< typename T>
    T GetDataT(s64 key)
    {
        const void *p = &key;
        if (p)
            return *(T*)p;
        return T();
    }
};

class Player : public tlib::linear::ILinkNode
{
public:
    Player(s32 id) :_id(id){};

    void Printf(){ ECHO("id = %d", _id); };
private:
    s32 _id;
};

class Test : public ITest
{
public:
    virtual ~Test(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
    virtual s32 Add(s32 a, s32 b){ return a + b; };
protected:

    void TestLinkList();

private:
    static Test     * s_self;
    static IKernel  * s_kernel;
};
#endif