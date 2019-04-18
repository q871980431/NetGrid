#include "Modulemgr.h"
#include "Tools.h"
#include "XmlReader.h"
#include "../Kernel.h"
#include "Configmgr.h"

IKernel * Modulemgr::s_kernel = nullptr;
bool Modulemgr::Ready()
{
    s_kernel = KERNEL;
    return true;
}

bool Modulemgr::Initialize()
{
    return LoadModule();
}

bool Modulemgr::Destroy()
{
    IKernel *kernel = s_kernel;
    for (auto iter : _modules)
    {
        if (!iter.second->Destroy(s_kernel))
        {
            TRACE_LOG("Module %s Destroy failed", iter.first.GetString());
            return false;
        }
        TRACE_LOG("Module %s Destroy Success", iter.first.GetString());
    }

    return true;
}

IModule * Modulemgr::FindModule(const char *name)
{
    auto iter = _modules.find(name);
    return iter == _modules.end() ? nullptr : iter->second;
}

bool Modulemgr::LoadModule()
{
    IKernel *kernel = s_kernel;
    const ModuleConfigs *config = Configmgr::GetInstance()->GetModuleConfig();
    char path[MAX_PATH];
    for (auto iter : config->vecModules)
    {
		SafeSprintf(path, sizeof(path), "%s/%s", tools::GetAppPath(), config->strModulePath.c_str());
        GetModuleFun fun = tools::LoadDynamicFun<GetModuleFun>(path, iter.c_str(), GET_LOGIC_FUN_NAME);
        if (fun == nullptr)
            return false;
        IModule *module = fun();
        ASSERT(module, "Can't Get Module from lib %s", path);
        while (module)
        {
            const char *name = module->GetName();
            auto iter = _modules.find(name);
            if (iter != _modules.end())
            {
                ASSERT(false, "Module %s Has Exist", name);
                return false;
            }
            _modules.insert(std::make_pair(name, module));
            module = module->GetNext();
        }
    }

    for (auto iter : _modules)
    {
        if (!iter.second->Initialize(s_kernel))
        {
            ASSERT(false, "module %s Initialize failed", iter.first.GetString());
            return false;
        }
    }
    for (auto iter : _modules)
    {
        if (!iter.second->Launched(s_kernel))
        {
            ASSERT(false, "module %s Launched failed", iter.first.GetString());
            return false;
        }
        TRACE_LOG("Module %s Launched Success", iter.first.GetString());
    }

    return true;
}