#ifndef __CDumper_h__
#define __CDumper_h__

#ifdef WIN32
#include "windows.h"
#include <DbgHelp.h>
#include "stdio.h"
#include <tchar.h>

class CDumper
{
public:
	static CDumper& GetInstance(void);
	~CDumper();
	void setDumpFileName(const TCHAR* dumpName){};
protected:
	static void CreateMiniDump(EXCEPTION_POINTERS* pep);

	static LONG _stdcall snUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo );

	static BOOL CALLBACK MiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput){};

private:
	CDumper(void);
	CDumper(const CDumper& rhs){}
private:
	LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;


};
#endif
#endif
