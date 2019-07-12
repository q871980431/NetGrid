#ifndef __ExceptionMgr_h__
#define __ExceptionMgr_h__
#include "MultiSys.h"
#include <setjmp.h>
#ifdef LINUX
#include <libunwind.h>
#endif
#include "TString.h"
#include "Tools_time.h"

typedef tlib::TString<8 * 1024> DumpBuff;

struct ExceptionJump
{
	jmp_buf env;
	ExceptionJump *next;
};

class ExceptionMgr
{
public:
	static void Init();

	static void PushJumpNode(ExceptionJump &expJump);
	static void PopJumpNode();
	static void DumpException(std::exception &e);
	static FILE  * OpenExceptionFile();
	static void DumpBackTrace(DumpBuff &dumpBuff);
protected:
private:
	static void CatchSignal(s32 sig);
	static void ExceptionHandFun();
	static void DumpBackTrace();
private:
};

#ifdef WIN32
	#define TRY_BEGIN
	#define TRY_END
#else
	#define TRY_BEGIN {\
		ExceptionJump jumpNode;\
		ExceptionMgr::PushJumpNode(jumpNode);\
		if (sigsetjmp(jumpNode.env, 1) == 0){\
			try{

	#define TRY_END \
			}\
			catch (std::exception &e) {\
				ExceptionMgr::DumpException(e);\
			}\
		}\
		ExceptionMgr::PopJumpNode();\
	}
#endif


#define DUMP_BACK_TRACE(code) \
DumpBuff dumpBuff;\
dumpBuff.AppendFormat("Catch Exception:%d,Time:%s\n", (code), tools::GetCurrentTimeString());\
ExceptionMgr::DumpBackTrace(dumpBuff);\
dumpBuff << "Exception Data End\n";\
FILE *dumpFile = ExceptionMgr::OpenExceptionFile();\
if (dumpFile)\
{\
	fwrite(dumpBuff.GetString(), dumpBuff.Length(), 1, dumpFile);\
	fflush(dumpFile);\
	fclose(dumpFile);\
}\




#endif
