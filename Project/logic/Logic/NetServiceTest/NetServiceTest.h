/*
 * File:	NetServiceTest.h
 * Author:	xuping
 * 
 * Created On 2019/4/30 16:44:29
 */

#ifndef __NetServiceTest_h__
#define __NetServiceTest_h__
#include "INetServiceTest.h"
#include "INetService.h"
#include "TString.h"
#include "ServiceSession.h"


class NetServiceTest : public INetServiceTest, public INetTcpListener
{
	struct ConfigInfo
	{
		bool isServer;
		tlib::TString<64> ip;
		s32 port;
		s32 clientNum;
		bool clientClose;
	};
public:
    virtual ~NetServiceTest(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

	virtual ITcpSession * CreateSession();
	virtual void OnRelease() {};
protected:
	void StartServer();
	void StartClient(core::IKernel *kernel);
	bool LoadConfigFile();

private:
    static NetServiceTest     * s_self;
    static IKernel  * s_kernel;
	static INetService *s_netService;


	static ConfigInfo s_configInfo;
};
#endif