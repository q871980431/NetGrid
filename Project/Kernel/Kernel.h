#ifndef __Kernel_h__
#define __Kernel_h__
#include "Singleton.h"
#define KERNEL_EXPORT
#include "IKernel.h"
#include "IApplication.h"
#include <string>
#include <unordered_map>
#include "Logger.h"
#include <vector>

class AsyncQueue;
class Kernel    : public core::IKernel, public core::IApplication, public Singleton<Kernel>
{
public:
    virtual bool Ready();
    virtual bool Initialize(s32 argc, char **argv);
    virtual void Loop();
    virtual void Destroy();
    virtual const char * GetCmdArg(const char *name);

public:
    virtual void SyncLog(const char *contens);
    virtual void AsyncLog(const char *contens);
	virtual void ThreadLog(const char *contents);
	virtual s32  GetLogLevel();
    virtual IModule * FindModule(const char *name);
    virtual void CreateNetSession(const char *ip, s16 port, core::ITcpSession *session);
    virtual void CreateNetListener(const char *ip, s16 port, core::INetTcpListener *listener);
	virtual void StartTimer(core::ITimer *timer, s64 delay, s32 count, s64 interval, const char *trace);
	virtual void KillTimer(core::ITimer *timer);
	virtual void AddFrame(core::IFrame *frame, u8 runLvl, const char *trace);
	virtual void RemoveFrame(core::IFrame *frame);
	virtual void StartAsync(const s64 threadId, core::IAsyncHandler * handler, const char * file, const s32 line);
	virtual void StopAsync(core::IAsyncHandler * handler);
	virtual core::IAsyncQueue * GetMainAsyncQueue();
	virtual core::IAsyncQueue * CreateAsyncQueue(const s32 threadSize, const char *trace);
    virtual const char* GetCoreFile();
    virtual const char* GetConfigFile();
    virtual const  char* GetEnvirPath();
	virtual const char * GetProcName() { return _procName.c_str(); };

protected:
private:
    void ParseCommand(s32 argc, char **argv);
	inline s32 GetNewQueueId() { return _asyncQueueId++; };
	AsyncQueue * CreateAsyncQueue(s32 queueId, const s32 threadSize, const char *trace);
private:
    std::unordered_map<std::string, std::string>    _cmdArgs;
    Logger                                          _logger;
	s32											_asyncQueueId;
	AsyncQueue								  * _mainQueue;
	std::unordered_map<s32, AsyncQueue*>	    _asyncQueues;
	std::string									_procName;
	s32											_frameNum;
};

#define KERNEL ((Kernel * )(Kernel::GetInstancePtr()))
//#define KERNEL Kernel::GetInstancePtr()
#endif