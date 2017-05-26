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
    class IKernel
    {
    public:
        virtual void SyncLog(const char *contens) = 0;
        virtual void AsyncLog(const char *contens) = 0;
        virtual IModule * FindModule(const char *name) = 0;
    };
}
#endif
