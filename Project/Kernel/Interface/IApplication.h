#ifndef __IApplication_h__
#define __IApplication_h__
#include "MultiSys.h"
namespace core
{
    class IApplication
    {
    public:
        virtual ~IApplication(){};
        virtual bool Ready() = 0;
        virtual bool Initialize(s32 argc, char **argv) = 0;
        virtual void Loop() = 0;
        virtual void Destroy() = 0;
    };
}

#endif