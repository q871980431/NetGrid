#ifndef __IConfigmgr_h__
#define __IConfigmgr_h__
#include "MultiSys.h"
#include "MultiSys.h"
#include "ICore.h"
#include <vector>
#include <string>

using namespace std;

struct CoreConfigs{


};

struct ModuleConfigs{
    string strModulePath;
    vector<string> vecModules;
};

class IConfigmgr : public core::ICore{
public:
    virtual ~IConfigmgr(){};

    virtual const CoreConfigs * GetCoreConfig() = 0;
    virtual const ModuleConfigs *GetModuleConfig() = 0;
    virtual const char* GetCoreFile() = 0;
    virtual const char* GetConfigFile() = 0;
    virtual const  char* GetEnvirPath() = 0;
};
#endif
