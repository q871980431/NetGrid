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
#include <vector>
typedef tlib::TString<core::NODE_NAME_LEN>		ServiceName;
typedef tlib::TString<core::NODE_CMD_LEN>		ServiceCMD;
const static s32 SERVICE_HEART_BEAT = 1000;
const static s32 START_TIMER_DELAY = 500;

class StartServiceTimer;
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
struct ServiceInfo;
struct  ServiceNodeInfo
{
	ServiceInfo *parent;

	s32 nodeId;
	s8  state;
};

struct ServiceInfo
{
	ServiceConfig config;
	s8 state;

	s32	count;
	std::vector<ServiceNodeInfo *> serviceNode;
	std::vector<StartServiceTimer *> startTimers;
};

typedef std::map<s8, ServiceInfo> ServiceMap;
typedef std::bitset<core::NODE_TYPE_MAX> ServiceFalg;
class ServiceGroup : public core::ITimer
{
public:
	ServiceGroup():_kernel(nullptr) {};
	~ServiceGroup() {};

	virtual void OnStart(core::IKernel *kernel, s64 tick) {};
	virtual void OnTime(core::IKernel *kernel, s64 tick);
	virtual void OnTerminate(core::IKernel *kernel, s64 tick) {};
public:
	void AddService(ServiceConfig *config);
	void StartService(core::IKernel *kernel);
	void UpdateServiceStatus(s32 serviceType, s32 serviceId, s8 state);
private:
	void OnServiceStatusChange(ServiceInfo *service, s32 nodeId, s8 status);
	void OnAllServiceReady();
	ServiceNodeInfo * CreateServiceNode(ServiceInfo *service);
protected:
private:
	core::IKernel	* _kernel;

	ServiceMap		_serviceMap;
	ServiceFalg	_serviceFlag;
	s8				_groupStatus;
};

class StartServiceTimer	: public core::ITimer
{
public:
	StartServiceTimer(ServiceNodeInfo *serviceNodeInfo) :_serviceNode(serviceNodeInfo) {};

	virtual void OnStart(core::IKernel *kernel, s64 tick) {};
	virtual void OnTime(core::IKernel *kernel, s64 tick);
	virtual void OnTerminate(core::IKernel *kernel, s64 tick);
protected:
private:
	const ServiceNodeInfo *_serviceNode;
};
#endif