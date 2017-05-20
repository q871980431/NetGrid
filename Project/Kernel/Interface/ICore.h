#ifndef __ICore_h__
#define __ICore_h__

#define CREATE_INSTANCE(ModuleName)             \
    static ModuleName *GetInstance(){           \
        static ModuleName *instance = nullptr;  \
        if (nullptr == instance)                \
        {                                       \
            instance = NEW ModuleName();        \
            if (!instance->Ready())             \
            {                                   \
                ASSERT(false, "config module cant be ready");\
                SAFE_DELETE(instance);          \
            }                                   \
        }                                       \
                                                \
        return instance;                        \
    }

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