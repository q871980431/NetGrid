/*
 * File:	Master.h
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:14:49
 */

#ifndef __ServiceGroup_h__
#define __ServiceGroup_h__
#include "MultiSys.h"
#include "TString.h"
#include "FrameworkDefine.h"
#include "IKernel.h"
#include <map>
#include <bitset>

typedef tlib::TString<core::NODE_NAME_LEN>		ServiceName;
typedef tlib::TString<core::NODE_CMD_LEN>		ServiceCMD;
struct ServiceConfig 
{
	s8		type;
	ServiceName name;
	ServiceCMD	cmd;
	bool	external;
	s8		slave;
	s16		min;
	s16		max;
};

struct  ServiceStatus
{
	s32 nodeId;
	s8  state;
};

struct ServiceInfo
{
	ServiceConfig config;
	s8 state;
	s32	count;
	ServiceStatus	*serviceStatus;
};

typedef std::map<s8, ServiceInfo> ServiceMap;
typedef std::bitset<core::NODE_TYPE_MAX> ServiceFalg;
class ServiceGroup
{
public:
	ServiceGroup() {};
	~ServiceGroup() {};

	void AddService(ServiceConfig *config);

	void StartService();

	void UpdateServiceStatus(s32 serviceType, s32 serviceId, s8 state);
private:
	void OnServiceStatusChange(ServiceInfo *service, s32 nodeId, s8 status);
	void OnAllServiceReady();
protected:
private:
	 ServiceMap		_serviceMap;
	 ServiceFalg	_serviceFlag;
	 s8				_groupStatus;
};

class ServiceTimer	: public core::ITimer
{
public:
	ServiceTimer(s8 slave, const char *cmd) :_slave(slave), _cmd(cmd){};

	virtual void OnStart(core::IKernel *kernel, s64 tick) {};
	virtual void OnTime(core::IKernel *kernel, s64 tick);
	virtual void OnTerminate(core::IKernel *kernel, s64 tick);
protected:
private:
	s8 _slave;
	ServiceCMD _cmd;
};
#endif