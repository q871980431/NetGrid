#ifndef __Lin_sys_h__
#define __Lin_sys_h__

#ifdef LINUX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <algorithm>
#include <string.h>
#include <pthread.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef char s8;
typedef short s16;
typedef int32_t s32;
typedef int64_t s64;

#define GLOBAL_VAR_INT_MAX __attribute__((init_priority(101)))
#define LOG_BUFF_SIZE   4096
//#define SafeSprintf snprintf
#define MSLEEP(n)	usleep(n*1000)
using std::max;
using std::min;


inline constexpr const char *getFileName(const char *filePath, const int len) { return len < 0 ? filePath : (filePath[len] == '/') ? filePath + len + 1 : getFileName(filePath, len - 1); }

#define ECHO(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "%s:%d:%s"#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    printf("%s\n", log);\
}


#define PTF_INFO(pszFmt,a...)\
{\
    char buff[4096]; \
    SafeSprintf(buff, sizeof(buff), "[INFO], [%s,%d], "#pszFmt, __FUNCTION__, __LINE__, ##a); \
    printf(buff); \
}

#define PTF_DEBUG(pszFmt,a...)\
{\
	char buff[4096]; \
	SafeSprintf(buff, sizeof(buff), "[DEBUG, [%s,%d], "#pszFmt, __FUNCTION__, __LINE__, ##a); \
	printf(buff); \
}

#define PTF_ERROR(pszFmt,a...)\
{\
	char buff[4096];\
	SafeSprintf(buff, sizeof(buff), "[ERROR, [%s,%d], "#pszFmt, __FUNCTION__, __LINE__, ##a); \
	printf(buff);\
}

#define ASSERT(_condition, _format, a...)\
{\
	if (!(_condition))\
	{\
		char buff[4096];\
		SafeSprintf(buff, sizeof(buff), "[ASSERT, [%s,%d], "#_format, __FUNCTION__, __LINE__, ##a);\
		printf(buff);\
		assert(false);\
	}\
}

#endif

#endif
