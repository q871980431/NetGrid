#include "Tools.h"
#include <string>
unsigned int  Radnom()
{
	static unsigned int  sRandom = 16807;
	sRandom = (214013 * sRandom + 2531011)&0x7FFFFFFF;
	return sRandom;
}
int GetRandom(int nA, int nB)
{
	if (nB < nA)std::swap(nA, nB);
	return nA + Radnom()% nB;
}
const char *format = "%4d-%02d-%02d %02d:%02d:%02d";
#ifdef __cplusplus
extern "C"  {
#endif
    const char * GetCurrentTimeString()
    {
        static char oldTime[64];
        static char newTime[64];
        static time_t t;
        static char *now = nullptr;

        if (nullptr == now)
        {
            tm *tm = nullptr;
            t = time(NULL);
            tm = localtime(&t);
            SafeSprintf(oldTime, sizeof(oldTime), format, tm->tm_year + 1900, tm->tm_mon + 1, \
                tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
            now = oldTime;
        }
        else
        {
            time_t t1 = time(NULL);
            tm *tm = nullptr;
            if (t1 - t > 0)
            {
                char *tmp = (now == oldTime) ? newTime : oldTime;
                tm = localtime(&t1);
                SafeSprintf(tmp, sizeof(oldTime), format, tm->tm_year + 1900, tm->tm_mon + 1, \
                    tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
                now = tmp;
            }
        }

        return now;
    }
#ifdef __cplusplus
}
#endif

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


    bool UpdateLocalTime(time_t time)
    {
#ifdef WIN32
        tm t;
        SYSTEMTIME st;
        localtime_s(&t, &time);
        st.wYear = t.tm_year + 1900;
        st.wMonth = t.tm_mon + 1;
        st.wDay = t.tm_mday;
        st.wHour = t.tm_hour;
        st.wMinute = t.tm_min;
        st.wSecond = t.tm_sec;
        st.wMilliseconds = 0;

        HANDLE hToken;
        TOKEN_PRIVILEGES tkp;
        //////////////////获得SE_SYSTEMTIME_NAME权限//////////////////
        // Get a token for this process. 
        if (!OpenProcessToken(GetCurrentProcess(),
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            return false;

        // Get the LUID for the shutdown privilege.
        //(获得SE_SYSTEMTIME_NAME权限，才可设置系统时间成功) 
        LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME,
            &tkp.Privileges[0].Luid);

        tkp.PrivilegeCount = 1;  // one privilege to set    
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        // Get the shutdown privilege for this process. 
        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,
            (PTOKEN_PRIVILEGES)NULL, 0);

        if (GetLastError() != ERROR_SUCCESS)
            return false;

        return (0 == ::SetLocalTime(&st)) ? false : true;
#elif defined LINUX
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);//获取时区保存tz中  
        tv.tv_sec = time;
        tv.tv_usec = 0;
        return (settimeofday(&tv, &tz) < 0) ? false : true;
#endif
    }

#ifdef __cplusplus
}
#endif

