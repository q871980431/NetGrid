#include "ServiceGroup.h"


void ServiceGroup::OnTime(core::IKernel *kernel, s64 tick)
{

}

void ServiceGroup::AddService(ServiceConfig *config)
{

	ServiceInfo info;
	info.config = *config;
	info.count = 0;
	info.serviceNode;
	info.state = core::NODE_STATUS_NONE;

	auto ret = _serviceMap.insert(std::make_pair(config->type, info));
	ASSERT(ret.second, "error");
}

void ServiceGroup::StartService(core::IKernel *kernel)
{
	_kernel = kernel;
	
	ADD_TIMER(this, SERVICE_HEART_BEAT, FOREVER, SERVICE_HEART_BEAT);
	if (_serviceMap.size() > 0)
	{
		for (auto &iter : _serviceMap)
		{

			_serviceFlag.set(iter.first);

			for (s16 i = 0; i < iter.second.config.min; i++)
			{
				ServiceNodeInfo *nodesInfo = CreateServiceNode(&iter.second);
				ASSERT(nodesInfo != nullptr, "error");
				if (nullptr != nodesInfo)
				{
					StartServiceTimer *timer = NEW StartServiceTimer(nodesInfo);
					iter.second.startTimers.push_back(timer);
					kernel->StartTimer(timer, START_TIMER_DELAY, FOREVER, START_TIMER_DELAY, "Serivce start timer");
				}
			}
		}
	}
	

}

void ServiceGroup::UpdateServiceStatus(s32 serviceType, s32 serviceId, s8 state)
{
	auto iter = _serviceMap.find(serviceType);
	ASSERT(iter != _serviceMap.end(), "error");
	if (iter != _serviceMap.end())
	{
		s32 count = 0;
		for ( auto nodes : iter->second.serviceNode)
		{
			if (nodes->nodeId == serviceId)
				nodes->state = state;
			if (nodes->state == core::NODE_STATUS_READY)
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

ServiceNodeInfo * ServiceGroup::CreateServiceNode(ServiceInfo *service)
{
	if (service->config.max != -1 && service->serviceNode.size() >= service->config.max)
	{
		ASSERT(false, "error");
	}
	if (service->config.max == -1 || service->serviceNode.size() < service->config.max)
	{
		ServiceNodeInfo *nodeInfo = NEW ServiceNodeInfo();
		nodeInfo->nodeId = ++service->count;
		nodeInfo->parent = service;
		nodeInfo->state = core::NODE_STATUS_NONE;
		service->serviceNode.push_back(nodeInfo);

		return nodeInfo;
	}
	return nullptr;
}

void StartServiceTimer::OnTime(core::IKernel *kernel, s64 tick)
{

}

void StartServiceTimer::OnTerminate(core::IKernel *kernel, s64 tick, bool isKill)
{

}