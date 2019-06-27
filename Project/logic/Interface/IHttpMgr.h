/*
 * File:	IHttpMgr.h
 * Author:	xuping
 * 
 * Created On 2019/6/27 15:58:14
 */

#ifndef  __IHttpMgr_h__
#define __IHttpMgr_h__
#include "IModule.h"

class IHttpBase {
public:
	virtual ~IHttpBase() {}
};

class IHttpHandler {
public:
	IHttpHandler() : _base(nullptr) {}
	virtual ~IHttpHandler() {}

	inline void SetBase(IHttpBase * base) { _base = base; }
	inline IHttpBase * GetBase() { return _base; }

	virtual void OnSuccess(IKernel * kernel, const void * context, const s32 size) = 0;
	virtual void OnFail(IKernel * kernel, const s32 errCode) = 0;
	virtual void OnRelease() = 0;

private:
	IHttpBase * _base;
};

class IHttpField
{
public:
	virtual ~IHttpField() {};

	virtual void SetInt8(const char *key, s8 val) = 0;
	virtual void SetInt16(const char *key, s16 val) = 0;
	virtual void SetInt32(const char *key, s32 val) = 0;
	virtual void SetInt64(const char *key, s64 val) = 0;
	virtual void SetFloat(const char *key, float val) = 0;
	virtual void SetString(const char *key, const char * val) = 0;
	virtual void SetBinary(const char *key, const void *context, s32 size) = 0;
};

typedef std::function<void(IKernel *kernel, IHttpField *fixField)> FieldFillFunc;

class IHttpMgr : public IModule
{
public:
    virtual ~IHttpMgr(){};
    
	virtual void Get(const s64 id, IHttpHandler * handler, const char * uri, const FieldFillFunc &reqArgsFun, const FieldFillFunc &cookieArgsFun = nullptr, bool htpps = false) = 0;
	virtual void Get(const s64 id, IHttpHandler * handler, const char * uri, const FieldFillFunc &cookieArgsFun = nullptr, bool htpps = false) = 0;
	virtual void Post(const s64 id, IHttpHandler * handler, const char * url, const FieldFillFunc &reqArgsFun, const FieldFillFunc &cookieArgsFun = nullptr, bool htpps = false) = 0;
	virtual void Post(const s64 id, IHttpHandler * handler, const char * url, const char *reqArgsContent, const FieldFillFunc &cookieArgsFun = nullptr, bool htpps = false) = 0;
	virtual void Stop(IHttpHandler * handler) = 0;
};
#endif