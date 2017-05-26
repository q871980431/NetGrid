#ifndef __NetEngine_h__
#define __NetEngine_h__
#include "INetEngine.h"
class NetEngine : public INetEngine
{
public:
    CREATE_INSTANCE(NetEngine);

    virtual bool Ready();
    virtual bool Initialize();
    virtual bool Destroy();
public:
    virtual void Process(s32 tick);
private:
};

#endif