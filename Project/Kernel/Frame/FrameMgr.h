#ifndef __FrameMgr_h__
#define __FrameMgr_h__
#include "IFrameMgr.h"

class FrameMgr : public IFrameMgr
{
public:
    CREATE_INSTANCE(FrameMgr);

	virtual bool Ready();
	virtual bool Initialize() { return true; };
	virtual bool Destroy() { return true; };

    virtual void Process(s32 tick);
	virtual void AddFrame(core::IFrame *framer, u8 runLvl);
	virtual void DelFrame(core::IFrame *framer);
protected:
private:
	core::IKernel   * _kernel;
};
#endif
