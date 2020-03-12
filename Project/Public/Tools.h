#ifndef __TOOLS_H__
#define __TOOLS_H__
#include "MultiSys.h"
#ifdef WIN32
#include <shlwapi.h>
#include<process.h>
#endif
#ifdef LINUX
#include<libgen.h>
#include<dlfcn.h>
#include <sys/types.h>
#endif
#include <random>

#define SETBIT(x, y) ((x) |= (1<<(y)))
#define CLRBIT(x, y) ((x) &= ~(1<<(y)))
#define GETBIT(x, y) (((x) & (1<<(y))) ? true : false)

namespace tools{

#pragma  pack (push, 4)             //按4字节对齐
	typedef union _KEY_INT64{
			struct  
			{
                s32 lVal;
				s32 hVal;
			};
			s64 val;
	}KEYINT64;

	typedef union _KEY_INT32
	{
		struct  
		{
            s16 lVal;
			s16 hVal;
		};
		s32 val;
	}KEYINT32;
#pragma  pack (pop)                 //取消知道对齐, 恢复缺省对齐

	inline u32  Random()
	{
		static std::random_device dev;
		static std::default_random_engine eng(dev());
		static std::uniform_int_distribution<u32> distribution;
		return distribution(eng);
	}
	inline s32 GetPid() { return getpid(); };

    inline void Mkdir(const char *path)
    {
#ifdef WIN32
        CreateDirectory(path, NULL);
#elif defined LINUX
        mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    }


    inline void SafeMemset(void *__restrict dest, s32 size, s8 val, s32 num)
	{
		ASSERT(size >= num, "out of rang");
		if (num > size)
			num = size;
		memset(dest, val, max(0, min(size, num)));
	}

    inline void SafeMemcpy(void * __restrict dest, s32 size, const void * __restrict source, s32 num)
    {
        ASSERT(size >= num, "out of rang");
        if (num > size)
            num = size;
        memcpy(dest, source, max(0, min(size, num)));
    }

    inline s32 SafeStrcpy(char * __restrict dest, s32 size, const char * __restrict src, s32 n)
    {
		size -= 1;
        ASSERT(n < size, "out of rang");
        n = max(0, min(n, size));
        memcpy(dest, src, n);
        dest[n] = 0;
        return n;
    }

	inline bool	 StrToBool(const char *val) { return (val != nullptr) ? (atoi(val) != 0) : false; }
	inline s8	 StrToInt8(const char *val) { return (val != nullptr) ? (s8)atoi(val) : 0; };
	inline s16	 StrToInt16(const char *val) { return (val != nullptr) ? (s16)atoi(val) : 0; };
	inline s32	 StrToInt32(const char *val) { return (val != nullptr) ? atoi(val) : 0; };
	inline s64   StrToInt64(const char *val) { return (val != nullptr) ? atoll (val) : 0; };
	inline float StrToFloat(const char *val) { return (val != nullptr) ? (float)atof(val) : 0.0f; };

	template< typename T>
	void ValToStr(char *buff, s32 buffSize, const T &val){SafeSprintf(buff, buffSize, "%d", val);}
	template<>
	inline void ValToStr(char *buff, s32 buffSize, const float &val){SafeSprintf(buff, buffSize, "%f", val);}
	template<>
	inline void ValToStr<s64>(char *buff, s32 buffSize, const s64 &val){SafeSprintf(buff, buffSize, "%ld", val);}
	template<>
	inline void ValToStr<u64>(char *buff, s32 buffSize, const u64 &val) { SafeSprintf(buff, buffSize, "%lu", val); }
	template<typename T>
	inline void Zero(T &val)
	{
		tools::SafeMemset(&val, sizeof(T), 0, sizeof(T));
	}
	template<typename T>
	bool Desc(const T &lVal, const T &rVal)
	{
		return lVal > rVal;
	}

	template<typename T>
	bool Asc(const T &lVal, const T &rVal)
	{
		return lVal < rVal;
	}

    //format = "%4d-%02d-%02d %02d:%02d:%02d"
#ifdef __cplusplus
    extern "C"{
#endif
        const char * GetAppPath();		
        s32 HashKey(const char *content);
        s32 GetRandom(s32 nA, s32 nB);
#ifdef __cplusplus
    }
#endif
    
    template<class InputIterator1, class InputIterator2, class OutputIterator1, class OutputIterator2 >
    void Difference(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator1 out1, OutputIterator2 out2)
    {
        while (first1 != last1 || first2 != last2)
        {
            if (first1 == last1)
            {
                while (first2 != last2){
                    *out2 = *first2; ++out2; ++first2;
                }
                return;
            }
            if (first2 == last2)
            {
                while (first1 != last1){
                    *out1 = *first1; ++out1; ++first1;
                }
                return;
            }
            if (*first1 < *first2) {
                    *out1 = *first1; ++out1; ++first1;
            }else if (*first2 < *first1) {
                    *out2 = *first2; ++out2; ++first2;
            }else { ++first1; ++first2; }
        }
    }


    template<class InputIterator1, class InputIterator2, class OutputIterator1, class OutputIterator2, class InterIterator>
    void DifferenceAndInter(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, OutputIterator1 out1, OutputIterator2 out2, InterIterator inter)
    {
        while (first1 != last1 || first2 != last2)
        {
            if (first1 == last1)
            {
                while (first2 != last2){
                    *out2 = *first2; ++out2; ++first2;
                }
                return;
            }
            if (first2 == last2)
            {
                while (first1 != last1){
                    *out1 = *first1; ++out1; ++first1;
                }
                return;
            }
            if (*first1 < *first2) {
                *out1 = *first1; ++out1; ++first1;
            }
            else if (*first2 < *first1) {
                *out2 = *first2; ++out2; ++first2;
            }
            else { *inter = *first1; ++first1;  ++first2; }
        }
    }
    template<typename FUN>
    FUN LoadDynamicFun(const char *path, const char *dllName, const char *funName, bool global = false)
    {
        FUN fun = nullptr;
        char filePath[MAX_PATH];

#ifdef LINUX
        SafeSprintf(filePath, sizeof(filePath), "%s/lib%s.so", path, dllName);
        void *handle = dlopen( filePath, global ? RTLD_LAZY | RTLD_GLOBAL :  RTLD_LAZY);
        ASSERT(handle, "open %s error %s", filePath, dlerror());
        fun = (FUN)dlsym(handle, funName);
        ASSERT(fun, "get function error");
#endif

#ifdef WIN32
        SafeSprintf(filePath, sizeof(filePath), "%s/%s.dll", path, dllName);
        HINSTANCE instance = ::LoadLibrary(filePath);
        ASSERT(instance != NULL, "open %s error %d", filePath, errno);
        fun = (FUN)::GetProcAddress(instance, funName);
        ASSERT(fun, "get function error");
#endif
        return fun;
    }

    template<typename T>
    struct HashFun{
        size_t operator()(const T &val) const{
            return (size_t)val;
        }
    };

    template<typename T>
    struct CompFun{
        bool operator()(const T &src, const T &dst) const{
            return src == dst;
        }
    };

	inline u8 ToHex(u8 x) { return x > 9 ? x + 55 : x + 48; };
	inline std::string UrlEncode(const char *context, const s32 size, char *dst)
	{
		std::string temp("");
		for (s32 i = 0; i < size; i++)
		{
			if (isalnum((u8)context[i])||
				(context[i] == '-') ||
				(context[i] == '_') ||
				(context[i] == '.') ||
				(context[i] == '~') )
				temp += context[i];
			else
			{
				temp += '%';
				temp += ToHex((u8)context[i] >> 4);
				temp += ToHex((u8)context[i] % 16);
			}
		}

		return temp;
	}

	//Thread safe by C++11
	template<typename T>
	class Singleton
	{
	public:
		static T & Instance()
		{
			static T singleton;
			return singleton;
		}
	private:
		Singleton() {};
	};
}

#endif 
