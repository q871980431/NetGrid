#ifndef  __Configmgr_h__
#define  __Configmgr_h__

#include "IConfigmgr.h"
#include "Singleton.h"
#include "TString.h"

namespace core
{
    static  const char *MODULE_PATH = "path";
    static  const char *MODULE_NAME = "module";
    static  const char *MODULE_NAME_ATTR = "name";
}

class Configmgr : public IConfigmgr
{
    typedef tlib::TString<MAX_PATH> FilePath;

public:
    CREATE_INSTANCE(Configmgr);

	virtual bool Ready();
    virtual bool Initialize();
    virtual bool Destroy();

    virtual inline const CoreConfigs * GetCoreConfig(){ return &_coreConfig; };
    virtual inline const ModuleConfigs * GetModuleConfig(){ return &_moduleConfig; };

	virtual const char* GetCoreFile() { return _coreFile.GetString();  }
	virtual const char* GetConfigFile(){ return _configFile.GetString();  }
	virtual const char* GetEnvirPath() { return _envirPath.GetString(); }

private:
	bool LoadCoreConfig();
	bool LoadModuleConfig();
private:
    Configmgr(){};
    virtual ~Configmgr(){};
private:
	CoreConfigs         _coreConfig;
	ModuleConfigs       _moduleConfig;
    FilePath            _coreFile;
    FilePath            _configFile;
    FilePath            _envirPath;
};

#endif