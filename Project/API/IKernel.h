#ifndef __IKernel_h__
#define __IKernel_h__
#include "MultiSys.h"
#include "CircularBuffer.h"
#include <set>

#ifdef WIN32
#ifdef KERNEL_EXPORT
#define KERNEL_API __declspec(dllexport)
#else
#define KERNEL_API __declspec(dllimport)
#endif
#else
#define KERNEL_API
#endif

#define  FIND_MODULE(module, name) {\
    module = (I##name * )(core::G_KERNEL::g_kernel->FindModule(#name));\
	ASSERT(module != nullptr, "don't find module,module name = %s", #name);\
}

#define FOREVER	-1


#define LUA_LOG(content)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[LUA]: %s", (content)); \
    core::G_KERNEL::g_kernel->AsyncLog(log);\
}

#ifdef WIN32

#define DEBUG_LOG(format, ...)\
{\
	if(  core::LOG_LEVEL_DEBUG >= core::G_KERNEL::g_kernel->GetLogLevel())\
	{\
		char log[LOG_BUFF_SIZE] = { 0 }; \
		SafeSprintf(log, sizeof(log), "[DEBUG]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
		core::G_KERNEL::g_kernel->AsyncLog(log);\
	}\
}

#define TRACE_LOG(format, ...)\
{\
	if(  core::LOG_LEVEL_TRACE >= core::G_KERNEL::g_kernel->GetLogLevel())\
	{\
		char log[LOG_BUFF_SIZE] = { 0 }; \
		SafeSprintf(log, sizeof(log), "[TRACE]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
		printf("%s\n", log);\
		core::G_KERNEL::g_kernel->AsyncLog(log); \
	}\
}

#define ERROR_LOG(format, ...)\
{\
	if(  core::LOG_LEVEL_DEBUG >= core::G_KERNEL::g_kernel->GetLogLevel())\
	{\
		char log[LOG_BUFF_SIZE] = { 0 }; \
		SafeSprintf(log, sizeof(log), "[ERROR]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
		core::G_KERNEL::g_kernel->AsyncLog(log); \
	}\
}

#define IMPORTANT_LOG(labl, format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[%s]: %s:%d:%s | "#format, labl, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    core::G_KERNEL::g_kernel->SyncLog(log); \
}

#define THREAD_LOG(labl,format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[%s]: %s:%d:%s | "#format, labl, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
	printf("%s\n", log);\
    core::G_KERNEL::g_kernel->ThreadLog(log); \
}

#endif
#ifdef LINUX
#define DEBUG_LOG(format, a...)\
{\
    	char log[LOG_BUFF_SIZE] = { 0 }; \
        SafeSprintf(log, sizeof(log), "[DEBUG]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	    core::G_KERNEL::g_kernel->AsyncLog(log);\
}

#define TRACE_LOG(format, a...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[TRACE]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##a); \
    core::G_KERNEL::g_kernel->AsyncLog(log); \
}


#define ERROR_LOG(format, a...)\
{\
	char log[LOG_BUFF_SIZE] = { 0 }; \
	SafeSprintf(log, sizeof(log), "[ERROR]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	core::G_KERNEL::g_kernel->AsyncLog(log); \
}

#define IMPORTANT_LOG(labl, format, a...)\
{\
	char log[LOG_BUFF_SIZE] = { 0 }; \
	SafeSprintf(log, sizeof(log), "[%s]: %s:%d:%s | "#format, labl, __FILE__, __LINE__, __FUNCTION__, ##a); \
	core::G_KERNEL::g_kernel->SyncLog(log); \
}

#define THREAD_LOG(labl,format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[%s]: %s:%d:%s | "#format, labl, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    core::G_KERNEL::g_kernel->ThreadLog(log); \
}

#endif
#ifdef WIN32
typedef SOCKET NetSocket;
#endif

#ifdef LINUX
typedef int NetSocket;
#endif

class IModule;
namespace core
{
    class IKernel;
	struct KERNEL_API G_KERNEL
	{
		static IKernel *g_kernel;
	};

	enum LOG_LEVEL
	{
		LOG_LEVEL_DEBUG = 0,
		LOG_LEVEL_TRACE = 1,
		LOG_LEVEL_ERROR = 2,
	};
    struct MessageHead
    {
        s32 messageId;
        s32 len;
    };

	class ITcpConnection
	{
	public:
		const static s32 INIT_SEND_BUFF_SIZE = 1 * 1024;
		const static s32 INIT_RECV_BUFF_SIZE = 1 * 1024;
		const static s32 CLOSE_DELAY_TIME = 5;					//延迟关闭5S

		virtual bool  IsPassiveConnection(void) = 0;			//是否是被动链接
		virtual void  Send(const char *buff, s32 len) = 0;      //
		virtual void  Close(const char *reason) = 0;                              //关闭连接
		virtual const char * GetRemoteIP() = 0;
		virtual s32   GetRemoteIpAddr() = 0;
		virtual bool  SettingBuffSize(s32 recvSize, s32 sendSize) = 0;	//设置时 需保证无数据发送和接收, 可以在SetConnection时进行调用
		virtual s32   GetSessionId() = 0;
		virtual const char * GetCloseReason() = 0;				//获取关闭的原因, 不可能为nullptr
	};

	class ITcpSession
	{
	public:
		const static s32 INVALID_PACKET_LEN = -1;
		const static s32 PACKET_UNFULL = 0;

		virtual void  SetConnection(ITcpConnection *connection) = 0;
		virtual void  OnEstablish() = 0;
		virtual void  OnTerminate() = 0;
		virtual void  OnError(s32 moduleErr, s32 sysErr) = 0;
		virtual void  OnRecv(const char *buff, s32 len) = 0;
		virtual s32	  OnParsePacket(CircluarBuffer *recvBuff) = 0;
		virtual void  OnRelease() = 0;
	};

	class INetTcpListener
	{
	public:
		virtual ITcpSession * CreateSession() = 0;
		virtual void OnRelease() = 0;
	};

    class ITrace
    {
    public:
        virtual ~ITrace(){};
        virtual const char * GetTraceInfo() = 0;
    };

    class ITimer
    {
    public:
        ITimer() :_base(nullptr){};
        inline void SetBase(ITrace *base){ _base = base; };
        inline ITrace * GetBase(){ return _base; };

    public:
        virtual void OnStart(IKernel *kernel, s64 tick) = 0;
        virtual void OnTime(IKernel *kernel, s64 tick) = 0;
        virtual void OnTerminate(IKernel *kernel, s64 tick, bool isKill) = 0;

    private:
        ITrace  *_base;
    };

	class IAsyncHandler {
	public:
		IAsyncHandler():_base(nullptr){}
		virtual ~IAsyncHandler() {}

		inline void SetBase(ITrace * base) { _base = base; }
		inline ITrace * GetBase() { return _base; }

		virtual bool OnExecute(IKernel * kernel, s32 queueId, s32 threadIdx) = 0;
		virtual void OnSuccess(IKernel * kernel) = 0;
		virtual void OnFailed(IKernel * kernel, bool isExecuted) = 0;
		virtual void OnRelease(IKernel * kernel) = 0;
	private:
		ITrace * _base;
	};

	class IAsyncQueue
	{
	public:
		virtual ~IAsyncQueue() {};

		virtual void StartAsync(const s64 threadId, IAsyncHandler * handler, const char * file, const s32 line) = 0;
		virtual void StopAsync(IAsyncHandler * handler) = 0;
		virtual s32  GetQueueId() = 0;
		virtual void  GetQueueInfo(s32 &threadNum, std::set<s32> &threadIds) = 0;
	};

	class IFrame
	{
	public:
		IFrame() :_base(nullptr) {};

		inline void SetBase(ITrace *base) { _base = base; };
		inline ITrace * GetBase() { return _base; };
	public:
		virtual void OnLoop(IKernel *kernel, s64 tick) = 0;
	private:
		ITrace  *_base;
	};



    class IKernel
    {
    public:
		//Log
        virtual void SyncLog(const char *contens) = 0;
        virtual void AsyncLog(const char *contens) = 0;
		virtual void ThreadLog(const char *contents) = 0;
		virtual s32  GetLogLevel() = 0;

		//Module
        virtual IModule * FindModule(const char *name) = 0;

        //Net
		virtual void CreateNetSession(const char *ip, s16 port, core::ITcpSession *session) = 0;
        virtual void CreateNetListener(const char *ip, s16 port, core::INetTcpListener *listener) = 0;
		
		//Timer
		virtual void StartTimer(core::ITimer *timer, s64 delay, s32 count, s64 interval, const char *trace) = 0;
		virtual void KillTimer(core::ITimer *timer) = 0;
		
		//Frame
		virtual void AddFrame(core::IFrame *frame, u8 runLvl, const char *trace) = 0;
		virtual void RemoveFrame(core::IFrame *frame) = 0;
        
		//AsyncTask
		virtual void StartAsync(const s64 threadId, IAsyncHandler * handler, const char * file, const s32 line) = 0;
		virtual void StopAsync(IAsyncHandler * handler) = 0;
		virtual IAsyncQueue * GetMainAsyncQueue() = 0;
		virtual IAsyncQueue * CreateAsyncQueue(const s32 threadSize, const char *trace) = 0;

		virtual const char* GetCoreFile() = 0;
        virtual const char* GetConfigFile() = 0;
        virtual const  char* GetEnvirPath() = 0;
        virtual const char * GetCmdArg(const char *name) = 0;
    };
}

#define ADD_TIMER(timer, delay, count, interval) {\
	char trace[256];\
	SafeSprintf(trace, sizeof(trace), "trace: %s:%d", __FILE__, __LINE__);\
	core::G_KERNEL::g_kernel->StartTimer(timer, delay, count, interval, trace);\
}

#define ADD_FRAME(frame, runLvl){\
	char trace[256];\
	SafeSprintf(trace, sizeof(trace), "trace: %s:%d", __FILE__, __LINE__);\
	core::G_KERNEL::g_kernel->AddFrame(frame, runLvl, trace);\
}

#define  START_ASYNC(id, handler){\
	core::G_KERNEL::g_kernel->StartAsync(id, handler, __FILE__, __LINE__);\
}

#endif
