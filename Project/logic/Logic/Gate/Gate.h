/*
 * File:	Gate.h
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:15:22
 */

#ifndef __Gate_h__
#define __Gate_h__
#include "IGate.h"


struct GateConfigInfo 
{
	std::string hostIp;
	s32 port;
	s32 heartBeat;
};

class Client;
class Gate : public IGate
{
public:
    virtual ~Gate(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
	
public:
	static void OnClientEnter(Client *client);
	static void OnClientMsg(Client *client, const char *content, s32 len);
	static void OnClientLeave(Client *client);

private:
	bool LoadConfig();

protected:
private:
    static Gate     * s_self;
    static IKernel  * s_kernel;
	static GateConfigInfo s_configInfo;
};
#endif