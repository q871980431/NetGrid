#ifndef __IKernel_h__
#define __IKernel_h__
#include "MultiSys.h"
#include "CircularBuffer.h"

#define  FIND_MODULE(module, name) {\
    module = (I##name * )(s_kernel->FindModule(#name));\
	ASSERT(module != nullptr, "don't find module,module name = %s", #name);\
}
#define FOREVER	-1

#define LUA_LOG(content)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[LUA]: %s", (content)); \
    kernel->AsyncLog(log);\
}

#ifdef WIN32

#define DEBUG_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[DEBUG]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    kernel->AsyncLog(log);\
}

#define TRACE_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[TRACE]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
	printf("%s\n", log);\
    kernel->AsyncLog(log); \
}

#define ERROR_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[ERROR]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    kernel->AsyncLog(log); \
}

#define IMPORTANT_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[IMPORTANT]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    kernel->SyncLog(log); \
}

#define THREAD_LOG(labl,format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[%s]: %s:%d:%s | "#format, labl, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
	printf("%s\n", log);\
    kernel->ThreadLog(log); \
}

#endif
#ifdef LINUX
#define DEBUG_LOG(format, a...)\
{\
    	char log[LOG_BUFF_SIZE] = { 0 }; \
        SafeSprintf(log, sizeof(log), "[DEBUG]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	    kernel->AsyncLog(log);\
}

#define TRACE_LOG(format, a...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[TRACE]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##a); \
    kernel->AsyncLog(log); \
}


#define ERROR_LOG(format, a...)\
{\
	char log[LOG_BUFF_SIZE] = { 0 }; \
	SafeSprintf(log, sizeof(log), "[ERROR]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	kernel->AsyncLog(log); \
}

#define IMPORTANT_LOG(format, a...)\
{\
	char log[LOG_BUFF_SIZE] = { 0 }; \
	SafeSprintf(log, sizeof(log), "[IMPORTANT]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	kernel->SyncLog(log); \
}

#define THREAD_LOG(labl,format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[%s]: %s:%d:%s | "#format, labl, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    kernel->ThreadLog(log); \
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
		virtual void  Close() = 0;                              //关闭连接
		virtual const char * GetRemoteIP() = 0;
		virtual s32   GetRemoteIpAddr() = 0;
		virtual void  SettingBuffSize(s32 recvSize, s32 sendSize) = 0;	//设置时 需保证无数据发送和接收, 可以在SetConnection时进行调用
		virtual s32   GetSessionId() = 0;
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
        ITrace(){};
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
        virtual void OnTerminate(IKernel *kernel, s64 tick) = 0;
    private:
        ITrace  *_base;
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
        virtual void SyncLog(const char *contens) = 0;
        virtual void AsyncLog(const char *contens) = 0;
		virtual void ThreadLog(const char *contents) = 0;
        virtual IModule * FindModule(const char *name) = 0;
        virtual void CreateNetSession(const char *ip, s16 port, core::ITcpSession *session) = 0;
        virtual void CreateNetListener(const char *ip, s16 port, core::INetTcpListener *listener) = 0;
		virtual void StartTimer(core::ITimer *timer, s32 delay, s32 count, s32 interval, const char *trace) = 0;
		virtual void KillTimer(core::ITimer *timer) = 0;
		virtual void AddFrame(core::IFrame *frame, u8 runLvl, const char *trace) = 0;
		virtual void RemoveFrame(core::IFrame *frame) = 0;
        virtual const char* GetCoreFile() = 0;
        virtual const char* GetConfigFile() = 0;
        virtual const  char* GetEnvirPath() = 0;
        virtual const char * GetCmdArg(const char *name) = 0;
    };
}

#define ADD_TIMER(timer, delay, count, interval) {\
	char trace[256];\
	SafeSprintf(trace, sizeof(trace), "trace: %s:%d", __FILE__, __LINE__);\
	kernel->StartTimer(timer, delay, count, interval, trace);\
}

#define ADD_FRAME(frame, runLvl){\
	char trace[256];\
	SafeSprintf(trace, sizeof(trace), "trace: %s:%d", __FILE__, __LINE__);\
	kernel->AddFrame(frame, runLvl, trace);\
}
#endif
