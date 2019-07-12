#ifndef __FrameMgr_h__
#define __FrameMgr_h__
#include "IFrameMgr.h"
#include "TString.h"
#include <list>

class FrameBase : public core::ITrace
{
public:
	FrameBase(const char *debug):_trace(debug){}
	virtual ~FrameBase() {};
	virtual const char * GetTraceInfo() { return _trace.GetString(); };

	void Release() { DEL this; };
protected:
private:
	tlib::TString<TRACE_LEN> _trace;
};

class FrameMgr : public IFrameMgr
{
public:
    CREATE_INSTANCE(FrameMgr);

	virtual bool Ready();
	virtual bool Initialize() { return true; };
	virtual bool Destroy() { return true; };

    virtual void Process(s32 tick);
	virtual void AddFrame(core::IFrame *framer, u8 runLvl, const char *debug);
	virtual void DelFrame(core::IFrame *framer);
protected:
private:
	core::IKernel   * _kernel;
	std::list<core::IFrame *>	_frames;
};
#endif
