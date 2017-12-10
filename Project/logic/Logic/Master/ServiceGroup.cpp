#include "ServiceGroup.h"


void ServiceGroup::AddService(ServiceConfig *config)
{

	ServiceInfo info;
	info.config = *config;
	info.count = 0;
	info.serviceStatus = NEW ServiceStatus[config->max];
	info.state = core::NODE_STATUS_NONE;

	auto ret = _serviceMap.insert(std::make_pair(config->type, info));
	ASSERT(ret.second, "error");
}

void ServiceGroup::StartService()
{
	if (_serviceMap.size() > 0)
	{
		for (auto &iter : _serviceMap)
		{

			_serviceFlag.set(iter.first);
		}
	}
}

void ServiceGroup::UpdateServiceStatus(s32 serviceType, s32 serviceId, s8 state)
{
	auto iter = _serviceMap.find(serviceType);
	ASSERT(iter != _serviceMap.end(), "error");
	if (iter != _serviceMap.end())
	{
		ServiceStatus *head = iter->second.serviceStatus;
		s32 count = 0;
		for (s32 i = 0; i < iter->second.count; i++)
		{
			if (head[i].nodeId == serviceId)
				head[i].state = state;
			if (head[i].state == core::NODE_STATUS_READY)
				count++;
		}
		if (count >= iter->second.config.min)
		{
			if (_groupStatus != core::NODE_STATUS_RUNNING)
				OnServiceStatusChange(&iter->second, serviceId, core::NODE_STATUS_READY);
			else
				;
		}
		else if (count == 0)
		{
			OnServiceStatusChange(&iter->second, serviceId, core::NODE_STATUS_REFUSE);
		}
		else
		{
			OnServiceStatusChange(&iter->second, serviceId, core::NODE_STATUS_BUSY);
		}
	}
}

void ServiceGroup::OnServiceStatusChange(ServiceInfo *service, s32 nodeId, s8 status)
{
	if (status == core::NODE_STATUS_READY && service->state == core::NODE_STATUS_NONE)
	{
		service->state = core::NODE_STATUS_READY;
		_serviceFlag.reset(service->config.type);
		if (_serviceFlag.none())
		{
			OnAllServiceReady();
		}
	}
	else
	{
		_serviceFlag.set(service->config.type);
		_groupStatus = status;
	}
	
	
}
void ServiceGroup::OnAllServiceReady()
{

}


void ServiceTimer::OnTime(core::IKernel *kernel, s64 tick)
{

}

void ServiceTimer::OnTerminate(core::IKernel *kernel, s64 tick)
{

}