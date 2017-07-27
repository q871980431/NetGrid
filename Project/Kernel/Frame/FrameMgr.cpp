#include "FrameMgr.h"
#include "TString.h"
#include "../Kernel.h"

bool FrameMgr::Ready()
{
	_kernel = KERNEL;
	return true;
}
void FrameMgr::Process(s32 tick)
{

}

void FrameMgr::AddFrame(core::IFrame *framer, u8 runLvl)
{

}

void FrameMgr::DelFrame(core::IFrame *framer)
{

}