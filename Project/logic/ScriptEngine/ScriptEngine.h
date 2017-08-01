/*
 * File:	ScriptEngine.h
 * Author:	xuping
 * 
 * Created On 2017/8/1 21:33:16
 */

#ifndef __ScriptEngine_h__
#define __ScriptEngine_h__
#include "IScriptEngine.h"
class ScriptEngine : public IScriptEngine
{
public:
    virtual ~ScriptEngine(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static ScriptEngine     * s_self;
    static IKernel  * s_kernel;
};
#endif