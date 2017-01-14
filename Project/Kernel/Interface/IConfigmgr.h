#include "MultiSys.h"
#include "MultiSys.h"
#include "ICore.h"
#include <vector>
#include <string>

using namespace std;

struct sCoreConfig{


};

struct sModuleConfig{
	string strModulePath;
	vector<string> vecModules;
};

class IConfigmgr : public core::ICore{
public:
	virtual const sCoreConfig * GetCoreConfig() = 0;
	virtual const sModuleConfig *GetModuleConfig() = 0;
	virtual const char* GetCoreFile() = 0;
	virtual const char* GetConfigFile() = 0;
	virtual const  char* GetEnvirPath() = 0;
};