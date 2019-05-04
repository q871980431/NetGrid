/*
 * File:	IScriptEngine.h
 * Author:	xuping
 * 
 * Created On 2017/8/1 21:33:16
 */

#ifndef  __IScriptEngine_h__
#define __IScriptEngine_h__
#include "IModule.h"
#include "IDataInterchange.h"
#include <functional>
class IScriptEngine : public IModule
{
public:
    virtual ~IScriptEngine(){};
    virtual IDataInterchangeCaller * PrepareCall( const char *module, const char * func) = 0;
	virtual void Call(const char *module, const char * func, const IDataOutputFuncType &writeFun, const IDataCallBackFuncType &readFun) = 0;
    virtual void RegModuleFunc(const char *module, const char *func, const IDataInterchangeFuncType &f, const char *debug) = 0;
	virtual void CallScriptFunc(const char *module, const char *func, const IDataOutputFuncType &outPutFunc, const IDataCallBackFuncType &callBackFun) = 0;
	virtual void PrintLuaStack() = 0;
};
#endif