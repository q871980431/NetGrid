#include "FrameMgr.h"
#include "TString.h"
#include "../Kernel.h"
#include "Tools_time.h"

bool FrameMgr::Ready()
{
	_kernel = KERNEL;
	return true;
}

void FrameMgr::Process(s32 tick)
{
	for (auto &iter : _frames)
	{
		iter->OnLoop(KERNEL, tools::GetTimeMillisecond());
	}
}

void FrameMgr::AddFrame(core::IFrame *framer, u8 runLvl, const char *debug)
{
	ASSERT(framer->GetBase() == nullptr, "error had add this");
	if (framer->GetBase() == nullptr)
	{
		FrameBase *base = NEW FrameBase(debug);
		framer->SetBase(base);
		_frames.push_back(framer);
	}
}

void FrameMgr::DelFrame(core::IFrame *framer)
{
	ASSERT(framer->GetBase() != nullptr, "error don't add this");
	if (framer->GetBase() != nullptr)
	{
		FrameBase *base = (FrameBase*)framer->GetBase();
		base->Release();
		_frames.remove(framer);
	}
}