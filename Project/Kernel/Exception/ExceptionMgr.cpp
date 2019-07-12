#include "ExceptionMgr.h"
#include "segvcatch.h"
#include <signal.h>
#include <setjmp.h>
#include "Tools.h"
#include "../Kernel.h"
#ifdef LINUX
#include "libunwind.h"
#include <cxxabi.h>
#endif

thread_local static ExceptionJump *gt_rootJump = nullptr;
void ExceptionMgr::Init()
{
	//segvcatch::init_segv(nullptr);
	//segvcatch::init_fpe(nullptr);

	//segvcatch::init_segv(ExceptionHandFun);	//可以采用 在linux回调过程中 打印dump文件, 达到和原生捕获信号量一致的效果
	//segvcatch::init_fpe(ExceptionHandFun);
	signal(SIGFPE, CatchSignal);
	signal(SIGSEGV, CatchSignal);
}

void ExceptionMgr::PushJumpNode(ExceptionJump &expJump)
{
	expJump.next = gt_rootJump;
	gt_rootJump = &expJump;
}

void ExceptionMgr::PopJumpNode()
{
	if (gt_rootJump != nullptr)
		gt_rootJump = gt_rootJump->next;
}

void ExceptionMgr::DumpException(std::exception &e)
{
	ExceptionHandFun();
}

void ExceptionMgr::CatchSignal(s32 sig)
{
#ifdef LINUX
	DUMP_BACK_TRACE(sig);
	if (gt_rootJump != nullptr)
		siglongjmp(gt_rootJump->env, 1);
	else
		abort();
#endif
}

void ExceptionMgr::ExceptionHandFun()
{
#ifdef LINUX
	DUMP_BACK_TRACE(0);
	if (gt_rootJump != nullptr)
		siglongjmp(gt_rootJump->env, 1);
	else
		abort();
#endif
}

FILE * ExceptionMgr::OpenExceptionFile()
{
	tlib::TString<256> fileName;
	const char *appPath = tools::GetAppPath();
	const char *procName = Kernel::GetInstance().GetCmdArg("name");
	const char *procId = Kernel::GetInstance().GetCmdArg("id");
	fileName << appPath << "/runtime/exception/"<< procName << "_" << procId;
	fileName.AppendFormat("_PID_%d.txt", tools::GetPid());
	return fopen(fileName.GetString(), "a");
}

void ExceptionMgr::DumpBackTrace()
{
	DumpBuff dumpBuff;
	dumpBuff.AppendFormat("Catch Exception,Time:%s\n", tools::GetCurrentTimeString());
	DumpBackTrace(dumpBuff);
	dumpBuff << "\nException Data End\n";
	FILE *dumpFile = OpenExceptionFile();
	if (dumpFile)
	{
		fwrite(dumpBuff.GetString(), dumpBuff.Length(), 1, dumpFile);
		fflush(dumpFile);
		fclose(dumpFile);
	}
}


#ifdef LINUX
void ExceptionMgr::DumpBackTrace(DumpBuff &dumpBuff)
{
	unw_cursor_t cursor;
	unw_context_t context;
	unw_word_t ip;
	unw_word_t offset;

	char fname[256] = { 0 };

	unw_getcontext(&context);
	unw_init_local(&cursor, &context);

	while (unw_step(&cursor) > 0) {
		fname[0] = 0;
		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		char *name = fname;
		unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);
		s32 status;
		char * demangled = abi::__cxa_demangle(fname, nullptr, nullptr, &status);
		if (status == 0)
			name = demangled;

		//len = snprintf(stac, sizeof(stac), "0x%016lx: %s+0x%lx\n", ip, fname, offset);
		dumpBuff.AppendFormat("0x%016lx: %s+0x%lx\n", ip, name, offset);
	}
}
#endif

#ifdef WIN32
void ExceptionMgr::DumpBackTrace(DumpBuff &dumpBuff)
{
	dumpBuff.AppendFormat("%s", "windows next add");
}
#endif


