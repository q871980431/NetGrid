#ifndef __ICore_h__
#define __ICore_h__
namespace core
{
    class ICore
    {
    public:
        virtual ~ICore(){};

        virtual bool Ready() = 0;
        virtual bool Initialize() = 0;
        virtual bool Destroy() = 0;
    };
}
#endif