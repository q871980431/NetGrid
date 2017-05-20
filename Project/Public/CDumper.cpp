#include "CDumper.h"
#ifdef WIN32
#pragma comment( lib,"DbgHelp.Lib")

TCHAR m_acDumpName[256] = { _T("\0") };
CDumper& CDumper::GetInstance(void){
	static CDumper app;
	return app;
}

CDumper::~CDumper(void){
	if (NULL != m_previousFilter){
		SetUnhandledExceptionFilter(m_previousFilter);
		m_previousFilter = NULL;
	}
}

LONG _stdcall CDumper::snUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo){
	CDumper::CreateMiniDump(pExceptionInfo);

	wchar_t msg[512];
	wsprintfW(msg, L"Exception happened. Exception code is xuxinyu test");
	MessageBoxW(NULL, msg, L"Exception", MB_OK); //显示消息给用户


	exit(pExceptionInfo->ExceptionRecord->ExceptionCode);

	return EXCEPTION_EXECUTE_HANDLER;
}

void CDumper::CreateMiniDump(EXCEPTION_POINTERS *pep){
	HANDLE hFile = CreateFile(_T("MiniDump.dmp"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);
	if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE)){
		MINIDUMP_EXCEPTION_INFORMATION mdei;
		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pep;
		mdei.ClientPointers = FALSE;
		MINIDUMP_CALLBACK_INFORMATION mci;
		mci.CallbackRoutine = 0;
		mci.CallbackParam = 0;
		MINIDUMP_TYPE mdt = MiniDumpNormal;
		BOOL bOk = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci);
		CloseHandle(hFile);
	}
}

CDumper::CDumper(void){
	m_previousFilter = ::SetUnhandledExceptionFilter(CDumper::snUnhandledExceptionFilter );
}
#endif
