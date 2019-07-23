#ifndef __Convert_h__
#define __Convert_h__
#include "MultiSys.h"
inline s32 StrToInt32(const char *val){ ASSERT(val, "error"); return atoi(val); };
inline s64 StrToInt64(const char *val){ ASSERT(val, "error"); return atoll(val); };
inline float StrToFloat(const char *val){ ASSERT(val, "error"); return (float)atof(val); };
#endif