#ifndef __Muliti_Sys_h__
#define __Muliti_Sys_h__
#include "WinSys.h"
#include "LinSys.h"
#include <functional>
#include <stdarg.h>

#define NEW     new
#define DEL     delete
#define SAFE_DELETE(p) if((p)){DEL p; p = nullptr;}
#define SAFE_DELETE_ARRAY(p) if((p)){DEL[] p; p = nullptr;}
#define MAKE_BOOL(p)    ((0 == (p)) ? false : true)
#define DIR_DELIMITER   "/"
#define MAX_PATH	260
#define TRACE_LEN   512
#define IP_LEN      32
#define  SafeSprintf __SafeSprintf
#define TMALLOC	malloc
#define TFREE	free

enum DATA_TYPE
{
    DATA_TYPE_NULL      = 0,
    DATA_TYPE_BOOL      = 1,
    DATA_TYPE_S8        = 2,
    DATA_TYPE_S16       = 3,
    DATA_TYPE_S32       = 4,
    DATA_TYPE_S64       = 5,
    DATA_TYPE_FLOAT     = 6,
    DATA_TYPE_STR       = 7,
    DATA_TYPE_BINARY    = 8,
    DATA_TYPE_TABLE     = 9,
    DATA_TYPE_OBJ       = 10,
};

inline s32 __SafeSprintf(char *buf, s32 size, const char *fromat, ...)
#ifdef LINUX
__attribute__((format(printf, 3, 4)))
#endif
;
s32 __SafeSprintf(char *buf, s32 size, const char *fromat, ...)
{
	va_list argList;
	va_start(argList, fromat);
	s32 ret = vsnprintf(buf, size, fromat, argList);
	va_end(argList);
	return ret;
}

#endif
