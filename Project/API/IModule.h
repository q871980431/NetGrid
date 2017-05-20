#ifndef __IModule_h__
#define __IModule_h__
#include "IKernel.h"

using namespace core;
#define MODULE_NAME_LENGTH		(64)

class IModule
{
public:
    IModule(){ _next = nullptr; memset(_name, 0, sizeof(_name)); };
    virtual  ~IModule(){}
    virtual bool Initialize(IKernel *kernel) = 0;
    virtual bool Launched(IKernel *kernel) = 0;
    virtual bool Destroy(IKernel *kernel) = 0;
public:
    inline void SetNext(IModule *module){ _next = module; };
    inline IModule * GetNext(){ return _next; };
    void SetName(const char *name){ memcpy(_name, name, strlen(name)); }
    const char * GetName(){ return _name; };
private:
    IModule *_next;
    char _name[MODULE_NAME_LENGTH];
};

typedef IModule * (*GetModuleFun)(void);
#define GET_LOGIC_FUN   GetLogicModule
#define GET_LOGIC_FUN_NAME "GetLogicModule"

#define CREATE_MODULE(name)\
    class factroy##name\
    {\
        public:\
            factroy##name(IModule *&module)\
            {\
                IModule *tmp = new name(); \
                tmp->SetName(#name); \
                tmp->SetNext(module); \
                module = tmp;\
            }\
    }; \
    factroy##name factroy##name(g_module);

#ifdef WIN32
#define GET_DLL_ENTRANCE\
    static IModule * g_module = nullptr; \
    extern "C" __declspec(dllexport) IModule * __cdecl GET_LOGIC_FUN(){ return g_module; }\
    BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lovReserved){ return TRUE; }
#elif defined LINUX
#define GET_DLL_ENTRANCE\
    static IModule * g_module = nullptr; \
    extern "C"  IModule * GET_LOGIC_FUN(){ return g_module; }

#endif

#endif