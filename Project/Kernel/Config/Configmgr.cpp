#include "Configmgr.h"
#include "Tools.h"
#include "XmlReader.h"
#include "../Kernel.h"

bool Configmgr::Ready()
{

    return true;
}

bool Configmgr::Initialize()
{
    return LoadCoreConfig() && LoadModuleConfig();
}

bool Configmgr::Destroy()
{
    return true;
}

bool Configmgr::LoadCoreConfig()
{
    const char *moduleName = KERNEL->GetCmdArg("name");
    char path[MAX_PATH];
    SafeSprintf(path, sizeof(path), "%s/core/%s/config.xml", tools::GetAppPath(), moduleName);
    _configFile = path;

    SafeSprintf(path, sizeof(path), "%s/core/server_config.xml", tools::GetAppPath());
    _coreFile = path;

    XmlReader reader;
    if (!reader.LoadFile(path))
    {
        ASSERT(false, "don't load config, file = %s", path);
        return false;
    }
    IXmlObject *root = reader.Root();
    IXmlObject *env = root->GetFirstChrild("env");
    if (nullptr == env)
    {
        ASSERT(false, "don't find env node");
        return false;
    }
    _envirPath = env->GetAttribute_Str("path");

    return true;
}

bool Configmgr::LoadModuleConfig()
{
    const char *moduleName = KERNEL->GetCmdArg("name");
    char path[MAX_PATH];
    SafeSprintf(path, sizeof(path), "%s/core/%s/module.xml", tools::GetAppPath(), moduleName);

    XmlReader reader;
    if (!reader.LoadFile(path))
    {
        ASSERT(false, "don't load config, file = %s", path);
        return false;
    }
    IXmlObject *root = reader.Root();
    _moduleConfig.strModulePath = root->GetAttribute_Str(core::MODULE_PATH);
    IXmlObject *module = root->GetFirstChrild(core::MODULE_NAME);
    while (module)
    {
        _moduleConfig.vecModules.push_back(module->GetAttribute_Str(core::MODULE_NAME_ATTR));
        module = module->GetNextSibling();
    }

    return true;
}