#ifndef _ProfileMgr_h__
#define _ProfileMgr_h__
#include "IProfileMgr.h"
#include "Tools.h"

class ProfileMgr : public IProfileMgr
{
	struct ProfileConfig 
	{
		bool openMemInfo;
		bool openCpuInfo;
		s64  checkTime;
		s64  lastCheckTime;
	};
public:
	virtual ~ProfileMgr() {};

	virtual bool Ready();
	virtual bool Initialize();
	virtual bool Destroy();
	virtual void Process(s32 tick);

private:
	bool _LoadConfig();

private:
	ProfileConfig	_config;
};

#define PROFILEMGR tools::Singleton<ProfileMgr>::Instance()

#endif
