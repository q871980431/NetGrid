#ifndef __IKernel_h__
#define __IKernel_h__
#include "MultiSys.h"

#ifdef WIN32

#define DEBUG_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[DEBUG]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    _kernel->AsyncLog(log);\
}

#define ERROR_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[ERROR]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    _kernel->AsyncLog(log); \
}

#define IMPORTANT_LOG(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "[IMPORTANT]: %s:%d:%s | "#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    _kernel->SyncLog(log); \
}


#endif
#ifdef LINUX
#define DEBUG_LOG(format, a...)\
{\
    	char log[LOG_BUFF_SIZE] = { 0 }; \
        SafeSprintf(log, sizeof(log), "[DEBUG]: %s:%d:%s | " format, __FILE__, __LINE__, __FUNCTION__, ##a); \
	_kernel->AsyncLog(log);\
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
namespace core
{
    class IKernel
    {
    public:
        virtual void SyncLog(const char *contens) = 0;
        virtual void AsyncLog(const char *contens) = 0;
    };
}
#endif
