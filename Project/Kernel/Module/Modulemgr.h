#ifndef  __Modulemgr_h__
#define  __Modulemgr_h__

#include "IModulemgr.h"
#include "Singleton.h"
#include "TString.h"
#include "Tools.h"
#include <unordered_map>
namespace core
{
    //static  const char *MODULE_PATH = "path";
    //static  const char *MODULE_NAME = "module";
    //static  const char *MODULE_NAME_ATTR = "name";
}

class Modulemgr : public IModulemgr
{
    typedef tlib::TString<MODULE_NAME_LENGTH> ModuleName;
    typedef std::unordered_map<ModuleName, IModule *, tools::HashFun<ModuleName>, tools::CompFun<ModuleName>> ModuleMap;
public:
    CREATE_INSTANCE(Modulemgr);

	virtual bool Ready();
    virtual bool Initialize();
    virtual bool Destroy();
public:
    virtual IModule * FindModule(const char *name);

private:
	bool LoadModule();

private:
    Modulemgr(){};
    virtual ~Modulemgr(){};
private:
    static IKernel       * s_kernel;

    ModuleMap       _modules;
};

#endif