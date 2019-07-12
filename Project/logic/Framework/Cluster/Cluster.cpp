/*
 * File:	Cluster.cpp
 * Author:	xuping
 * 
 * Created On 2019/6/17 20:00:12
 */

#include "Cluster.h"
#include "test.pb.h"
Cluster * Cluster::s_self = nullptr;
IKernel * Cluster::s_kernel = nullptr;
bool Cluster::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	
	SearchRequest searchRequest;
	searchRequest.set_query("actor");
	searchRequest.set_page_num(1);
	std::string content;
	if (!searchRequest.SerializePartialToString(&content))
	{
		ASSERT(false, "error");
		return false;
	}
	SearchRequest temp;
	if (!temp.ParseFromArray(content.c_str(), content.size()))
	{
		ASSERT(false, "error");
		return false;
	}
	int page = temp.page_num();
	ECHO("PAGE:%d", page);
	
    return true;
}

bool Cluster::Launched(IKernel *kernel)
{

    return true;
}

bool Cluster::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}


