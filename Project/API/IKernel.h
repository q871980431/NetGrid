#ifndef __IKernel_h__
#define __IKernel_h__
#include "MultiSys.h"

#define  FIND_MODULE(module, name) {\
    module = (I##name * )(s_kernel->FindModule(#name));\
}

#ifdef WIN32

#define DEBUG_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[DEBUG]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    s_kernel->AsyncLog(log);\
}

#define TRACE_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[TRACE]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    s_kernel->AsyncLog(log); \
}

#define ERROR_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[ERROR]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    s_kernel->AsyncLog(log); \
}

#define IMPORTANT_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[IMPORTANT]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    s_kernel->SyncLog(log); \
}


#endif
#ifdef LINUX
#define DEBUG_LOG(format, a...)\
{\
    	char log[LOG_BUFF_SIZE] = { 0 }; \
        SafeSprintf(log, sizeof(log), "[DEBUG]: %s:%d:%s | " format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	_kernel->AsyncLog(log);\
}

#define TRACE_LOG(format, a...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[TRACE]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##a); \
    _kernel->AsyncLog(log); \
}


#define ERROR_LOG(format, a...)\
{\
	char log[LOG_BUFF_SIZE] = { 0 }; \
	SafeSprintf(log, sizeof(log), "[ERROR]: %s:%d:%s | " format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	_kernel->AsyncLog(log); \
}

#define IMPORTANT_LOG(format, a...)\
{\
	char log[LOG_BUFF_SIZE] = { 0 }; \
	SafeSprintf(log, sizeof(log), "[IMPORTANT]: %s:%d:%s | " format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	_kernel->SyncLog(log); \
}
#endif
class IModule;
namespace core
{
    class IMsgConnection
    {
    public:
        virtual bool  IsConnected(void) = 0;
        virtual void  Send(s32 messageId, const char *buff, s32 len) = 0;      //
        virtual void  Close() = 0;                              //πÿ±’¡¨Ω”
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

    class ITcpListener
    {
    public:
        virtual IMsgSession * CreateSession() = 0;
    };


    class IKernel
    {
    public:
        virtual void SyncLog(const char *contens) = 0;
        virtual void AsyncLog(const char *contens) = 0;
        virtual IModule * FindModule(const char *name) = 0;
        virtual void CreateNetSession(const char *ip, s16 port, core::IMsgSession *session) = 0;
        virtual void CreateNetListener(const char *ip, s16 port, core::ITcpListener *listener) = 0;
    };
}
#endif
