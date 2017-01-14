#ifndef  __Configmgr_h__
#define __Configmgr_h__

#include "IConfigmgr.h"
#include "TString.h"


class Configmgr : public IConfigmgr{
public:
	static IConfigmgr * getInstance();

	virtual bool Ready();
	virtual bool Inititalize();
	virtual bool Destory();

	virtual inline const sCoreConfig * GetCoreConfig();
	virtual inline const sModuleConfig * GetModuleConfig();

	virtual const char* GetCoreFile() { return _coreFile.c_str();  }
	virtual const char* GetConfigFile(){ return _configFile.c_str();  }
	virtual const char* GetEnvirPath() { return _envirPath.GetString(); }
private:
	bool LoadCoreConfig();
	bool LoadModuleConfig();
private:
	Configmgr();
	virtual ~Configmgr();
private:
	sCoreConfig m_oCoreConfig;
	sModuleConfig m_oModuleConfig;
	string _coreFile;
	string _configFile;
	TString<128> _envirPath;
};

#endif