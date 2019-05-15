#include "Tools.h"
#include <string>

#ifdef __cplusplus
extern "C" {
#endif
	const char * GetAppPath()
	{
		static char pathBuff[MAX_PATH];
		static char *path = nullptr;

		if (nullptr == path)
		{
			memset(pathBuff, 0, sizeof(pathBuff));
#ifdef WIN32
			GetModuleFileName(NULL, pathBuff, MAX_PATH);
			ECHO("begin pathBuff = %s", pathBuff);
			PathRemoveFileSpec(pathBuff);
			ECHO("end pathBuff = %s", pathBuff);
			path = pathBuff;
#elif  defined LINUX
			SafeSprintf(pathBuff, sizeof(pathBuff), "/proc/self/exe");
			s32 count = readlink(pathBuff, path, MAX_PATH);
			if (count >= MAX_PATH)
			{
				ASSERT(false, "system path error");
			}
			path = dirname(path);
#endif
        }
        return path;
    }



    s32 HashKey(const char *content)
    {
        s32 hash = 131;
        while (*content != '\0')
            hash += hash * 33 + *(content++);
        return hash;
    }

    s32 GetRandom(s32 nA, s32 nB)
    {
		if (nA == nB)return nA;

        if (nB < nA)std::swap(nA, nB);
        return nA + tools::Random() % (nB-nA);
    }


#ifdef __cplusplus
}
#endif

