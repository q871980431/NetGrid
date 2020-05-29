#ifndef __Winsys_h__
#define __Winsys_h__

#ifdef WIN32

#define NOMINMAX
#ifndef _WINSOCK2API_
#include <WinSock2.h>
#endif

#ifndef _WINDOWS_
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <algorithm>
#include <string.h>

#pragma  comment(lib, "ws2_32.lib")
#pragma  comment(lib, "shlwapi.lib")

typedef unsigned char u8;
typedef unsigned short u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef char s8;
typedef short s16;
typedef int32_t s32;
typedef int64_t s64;

#define LOG_BUFF_SIZE   2048
//#define SafeSprintf sprintf_s
#define MSLEEP(n)   Sleep(n)


#define CHECK_FMT(a, b)	__attribute__((format(printf, a, b)))
#define GLOBAL_VAR_INT_MAX 
using std::max;
using std::min;


inline constexpr const char *getFileName(const char *filePath, const int len) { return len < 0 ? filePath : (filePath[len] == '\\') ? filePath + len + 1 : getFileName(filePath, len - 1); }

#define ECHO(format, ...)\
{\
    char log[LOG_BUFF_SIZE] = { 0 }; \
    SafeSprintf(log, sizeof(log), "%s:%d:%s"#format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
    printf("%s\n", log); \
}
#define PTF(pszFmt,...)\
{\
	char buff[4096]; \
    	SafeSprintf(buff, sizeof(buff), "%s", ""##pszFmt"\n"); \
       	printf(buff, __VA_ARGS__); \
}


 #define PTF_INFO(pszFmt,...)\
{\
	char buff[4096]; \
    	SafeSprintf(buff, sizeof(buff), "%s", "[INFO], [%s,%d], "##pszFmt"\n"); \
       	printf(buff, __FUNCTION__, __LINE__, __VA_ARGS__); \
}

#define PTF_DEBUG(pszFmt,...)\
{\
	char buff[4096]; \
	SafeSprintf(buff, sizeof(buff), "%s", "[DEBUG], [%s,%d], "##pszFmt"\n"); \
	printf(buff, __FUNCTION__, __LINE__, __VA_ARGS__); \
}
		  
#define PTF_ERROR(pszFmt,...)\
{\
    	char buff[4096];\
     	SafeSprintf(buff, sizeof(buff), "%s", "[ERROR], [%s,%d], "##pszFmt"\n");\
	printf(buff, __FUNCTION__, __LINE__, __VA_ARGS__);\
}
		        
#define ASSERT(_condition, _format,...)\
{\
	if (!(_condition))\
	{\
		char buff[4096]; \
	        SafeSprintf(buff, sizeof(buff), "%s", "[ASSERT], [%s,%d], "##_format"\n");\
    		printf(buff, __FUNCTION__, __LINE__, __VA_ARGS__); \
         	assert(false); \
	}\
 }


#endif



#endif
