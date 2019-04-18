#ifndef __IKernel_h__
#define __IKernel_h__
#include "MultiSys.h"

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


#endif
#ifdef LINUX
#define DEBUG_LOG(format, a...)\
{\
    	char log[LOG_BUFF_SIZE] = { 0 }; \
        SafeSprintf(log, sizeof(log), "[DEBUG]: %s:%d:%s | " format, __FILE__, __LINE__, __FUNCTION__, ##a); \
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
	SafeSprintf(log, sizeof(log), "[ERROR]: %s:%d:%s | " format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	kernel->AsyncLog(log); \
}

#define IMPORTANT_LOG(format, a...)\
{\
	char log[LOG_BUFF_SIZE] = { 0 }; \
	SafeSprintf(log, sizeof(log), "[IMPORTANT]: %s:%d:%s | " format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	kernel->SyncLog(log); \
}
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
    class IMsgConnection
    {
    public:
        virtual bool  IsConnected(void) = 0;
        virtual void  Send(s32 messageId, const char *buff, s32 len) = 0;      //
        virtual void  SendBuff(const char *buff, s32 len) = 0;      //
        virtual void  Close() = 0;                              //关闭连接
        virtual const char * GetRemoteIP() = 0;
        virtual s32   GetRemoteIpAddr() = 0;
        virtual void SettingBuffSize(s32 recvSize, s32 sendSize) = 0;
    };

    class IMsgSession
    {
    public:
        virtual void  SetConnection(IMsgConnection *connection) = 0;
        virtual void  OnEstablish() = 0;
        virtual void  OnTerminate() = 0;
        virtual void  OnError(s32 moduleErr, s32 sysErr) = 0;
        virtual void  OnRecv(s32 messageId, const char *buff, s32 len) = 0;
    };

    class IMsgPipe
    {
    public:
        virtual void OnOpen() = 0;
        virtual void OnClose() = 0;
        virtual void OnRecv(s32 messageId, const char *buff, s32 len) = 0;
        virtual void  Send(s32 messageId, const char *buff, s32 len) = 0;      //
        virtual void  Close() = 0;                                              //关闭连接
    };

    class ITcpListener
    {
    public:
        virtual IMsgSession * CreateSession() = 0;
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
        virtual IModule * FindModule(const char *name) = 0;
        virtual void CreateNetSession(const char *ip, s16 port, core::IMsgSession *session) = 0;
        virtual void CreateNetListener(const char *ip, s16 port, core::ITcpListener *listener) = 0;
		virtual void StartTimer(core::ITimer *timer, s32 delay, s32 count, s32 interval, const char *trace) = 0;
		virtual void KillTimer(core::ITimer *timer) = 0;
		virtual void AddFrame(core::IFrame *frame, u8 runLvl) = 0;
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
#endif
