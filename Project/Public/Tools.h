#ifndef __TOOLS_H__
#define __TOOLS_H__
#include "MultiSys.h"
#ifdef WIN32
#include <shlwapi.h>
#endif

#ifdef LINUX
#include<libgen.h>
#endif

unsigned int  Radnom();
namespace tools{

#pragma  pack (push, 4)             //��4�ֽڶ���
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
#pragma  pack (pop)                 //ȡ��֪������, �ָ�ȱʡ����

    inline void Mkdir(const char *path)
    {
#ifdef WIN32
        CreateDirectory(path, NULL);
#elif defined LINUX
        mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    }
    template<typename T>
    inline void Zero( T &val)
    {
        tools::SafeMemset(&val, sizeof(T), 0, sizeof(T));
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

    inline s32 SafeStrcpy(char * __restrict dest, s32 max, const char * __restrict src, s32 n)
    {
        max -= 1;
        ASSERT(n <= max, "out of rang");
        n = max(0, min(n, max));
        memcpy(dest, src, n);
        dest[n] = 0;
        return n;
    }


    //format = "%4d-%02d-%02d %02d:%02d:%02d"
#ifdef __cplusplus
    extern "C"{
#endif
        const char * GetAppPath();
        s32 HashKey(const char *content);
        int GetRandom(int nA, int nB);
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
    FUN LoadDynamicFun(const char *path, const char *funName, bool global = false)
    {
        FUN fun = nullptr;
        char filePath[MAX_PATH];

#ifdef LINUX
        SafeSprintf(filePath, sizeof(filePath), "%s.so", path);
        void *handle = dlopen( filePath, global ? RTLD_LAZY | RTLD_GLOBAL :  RTLD_LAZY);
        ASSERT(handle, "open %s error %d", filePath, errno);
        fun = (FUN)dlsym(handle, funName);
        ASSERT(fun, "get function error");
#endif

#ifdef WIN32
        SafeSprintf(filePath, sizeof(filePath), "%s.dll", path);
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

}

#endif 
