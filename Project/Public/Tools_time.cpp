#include "Tools_time.h"

#ifdef LINUX
#include<sys/time.h>
#endif
const char *format = "%4d-%02d-%02d %02d:%02d:%02d";
#ifdef __cplusplus
extern "C"  {
#endif
//===============================================START===============================
    const char * GetCurrentTimeString(const char *timeFormat)
    {
		thread_local static char newTime[128];
		thread_local static time_t t = 0;

		thread_local const tm *tm = nullptr;
		if (timeFormat == nullptr)
			timeFormat = format;
        time_t t1 = tools::GetTimeSecond();
        if (t1 - t > 0)
        {
            tm = tools::SafeLocalTime(&t1);
            SafeSprintf(newTime, sizeof(newTime), timeFormat, tm->tm_year + 1900, tm->tm_mon + 1, \
                tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
        }

        return newTime;
    }

    bool UpdateLocalTime(time_t time)
    {
#ifdef WIN32
        tm t = *(tools::SafeLocalTime(&time));
        SYSTEMTIME st;
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

    time_t GetYearTime(s32 year, s32 month, s32 day, s32 hour, s32 min)
    {
        time_t now = tools::GetTimeSecond();
        tm time = *(tools::SafeLocalTime(&now));
        time.tm_mon = month - 1;
        time.tm_mday = day;
        time.tm_hour = hour;
        time.tm_min = min;
        time.tm_sec = 0;
        time.tm_year = year - 1900;

        return mktime(&time);
    }

    s32 GetGameWeek(time_t tick)
    {
        time_t zero = 0;
        const tm *time = tools::SafeLocalTime(&zero);
        tick += time->tm_hour * 3600;
        tick -= (GAME_FRESH_TIME * 3600);
        s32 day = tick / 86400;
        return (time->tm_wday + day - 1) / 7 + 1;
    }

//===================================END======================================
#ifdef __cplusplus
}
#endif

