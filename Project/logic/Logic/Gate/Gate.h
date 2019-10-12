/*
 * File:	Gate.h
 * Author:	Xuping
 * 
 * Created On 2017/9/1 14:15:22
 */

#ifndef __Gate_h__
#define __Gate_h__
#include "IGate.h"
#include "ClientSession.h"
#include "Client.h"
class Gate : public IGate
{
public:
    virtual ~Gate(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
	
	static void OnClientEnter(ClientSession *client);
	static void OnClientMsg();
	static void OnClientLeave(ClientSession *client);

protected:
private:
    static Gate     * s_self;
    static IKernel  * s_kernel;
};
#endif