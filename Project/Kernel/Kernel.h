#ifndef __Kernel_h__
#define __Kernel_h__
#include "Singleton.h"
#include "IKernel.h"
#include "IApplication.h"
#include <string>
#include <unordered_map>
#include "Logger.h"

class Kernel    : public core::IKernel, public core::IApplication, public Singleton<Kernel>
{
public:
    virtual bool Ready();
    virtual bool Initialize(s32 argc, char **argv);
    virtual void Loop();
    virtual void Destroy();
    virtual const char * GetCmdArg(const char *name);

    static bool ExecZone(s64 id, s64 objId){ return false; };

public:
    virtual void SyncLog(const char *contens);
    virtual void AsyncLog(const char *contens);
    virtual IModule * FindModule(const char *name);

protected:
private:
    void ParseCommand(s32 argc, char **argv);

private:
    std::unordered_map<std::string, std::string>    _cmdArgs;
    Logger                                          _logger;
};

#define KERNEL ((Kernel * )(Kernel::GetInstancePtr()))
//#define KERNEL Kernel::GetInstancePtr()
#endif