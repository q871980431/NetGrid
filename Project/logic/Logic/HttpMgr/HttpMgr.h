#ifndef __HTTPMGR_H__
#define __HTTPMGR_H__
//#include "util.h"
#include "IHttpMgr.h"
#include "curl.h"
#include <vector>

class HttpMgr : public IHttpMgr {
public:
    virtual bool Initialize(IKernel * kernel);
    virtual bool Launched(IKernel * kernel);
    virtual bool Destroy(IKernel * kernel);

	static HttpMgr * Self() { return s_self; }

	virtual void Get(const s64 id, IHttpHandler * handler, const char * uri, const FieldFillFunc &reqArgsFun, const FieldFillFunc &cookieArgsFun = nullptr, bool htpps = false);
	virtual void Get(const s64 id, IHttpHandler * handler, const char * uri, const FieldFillFunc &cookieArgsFun = nullptr, bool htpps = false);
	virtual void Post(const s64 id, IHttpHandler * handler, const char * url, const FieldFillFunc &reqArgsFun, const FieldFillFunc &cookieArgsFun = nullptr, bool htpps = false);
	virtual void Post(const s64 id, IHttpHandler * handler, const char * url, const char *reqArgsContent, const FieldFillFunc &cookieArgsFun = nullptr, bool htpps = false);
	virtual void Stop(IHttpHandler * handler);

private:
	static HttpMgr * s_self;
    static IKernel * s_kernel;

	static std::vector<CURL *> s_urls;
};


class ReqArgsField : public IHttpField
{
public:
	ReqArgsField(char *buff, s32 size) :_buff(buff), _size(size), _offSet(0), _first(true) {};
	virtual void SetInt8(const char *key, s8 val) { 
	};
	virtual void SetInt16(const char *key, s16 val) { FixFiled(key, val); };
	virtual void SetInt32(const char *key, s32 val) { FixFiled(key, val); };
	virtual void SetInt64(const char *key, s64 val) { FixFiled(key, val); };
	virtual void SetFloat(const char *key, float val) { FixFiled(key, val); };
	virtual void SetString(const char *key, const char * val) { FixFiled(key, val); };
	virtual void SetBinary(const char *key, const void *context, s32 size) {
		if (size>0)
		{
			char *dst = (char*)alloca((size / 3 + 1) * 4 + 1);
			//Base64Encode(context, size, dst, (size / 3 + 1) * 4 + 1);
			FixFiled(key, dst);
		}
	};

private:
	template<typename T>
	void FixFiled(const char *key, T val );
	void FixFiled(const char *key, s64 val);
	void FixFiled(const char *key, float val);
	void FixFiled(const char *key, const char *val);
private:
	char	*_buff;
	s32		_size;
	s32		_offSet;
	bool	_first;
};

template<typename T>
inline void ReqArgsField::FixFiled(const char *key, T val )
{
	if (_first)
	{
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "%s=%d", key, val);
		_first = false;
	}
	else
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "&%s=%d", key, val);
}
inline void ReqArgsField::FixFiled(const char *key, const char *val)
{
	if (_first)
	{
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "%s=%s", key, val);
		_first = false;
	}
	else
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "&%s=%s", key, val);
}

inline void ReqArgsField::FixFiled(const char *key, float val )
{
	if (_first)
	{
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "%s=%f", key, val);
		_first = false;
	}
	else
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "&%s=%f", key, val);
}

inline void ReqArgsField::FixFiled(const char *key, s64 val)
{
	if (_first)
	{
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "%s=%lld", key, val);
		_first = false;
	}
	else
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "&%s=%lld", key, val);
}

class CookieField : public IHttpField
{
public:
	CookieField(char *buff, s32 size) :_buff(buff), _size(size), _offSet(0), _first(true) {};
	virtual void SetInt8(const char *key, s8 val) { FixFiled(key, val); };
	virtual void SetInt16(const char *key, s16 val) { FixFiled(key, val); };
	virtual void SetInt32(const char *key, s32 val) { FixFiled(key, val); };
	virtual void SetInt64(const char *key, s64 val) { FixFiled(key, val); };
	virtual void SetFloat(const char *key, float val) { FixFiled(key, val); };
	virtual void SetString(const char *key, const char * val) { FixFiled(key, val); };
	virtual void SetBinary(const char *key, const void *context, s32 size) {
		if (size > 0)
		{
			char *dst = (char*)alloca((size / 3 + 1) * 4 + 1);
			//Base64Encode(context, size, dst, (size / 3 + 1) * 4 + 1);
			FixFiled(key, dst);
		}
	};

private:
	template<typename T>
	void FixFiled(const char *key, T val);
	void FixFiled(const char *key, s64 val);
	void FixFiled(const char *key, float val);
	void FixFiled(const char *key, const char *val);
private:
	char	*_buff;
	s32		_size;
	s32		_offSet;
	bool	_first;
};

template<typename T>
inline void CookieField::FixFiled(const char *key, T val)
{
	if (_first)
	{
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "%s=%d", key, val);
		_first = false;
	}
	else
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "; %s=%d", key, val);
}
inline void CookieField::FixFiled(const char *key, const char *val)
{
	if (_first)
	{
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "%s=%s", key, val);
		_first = false;
	}
	else
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "; %s=%s", key, val);
}

inline void CookieField::FixFiled(const char *key, float val)
{
	if (_first)
	{
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "%s=%f", key, val);
		_first = false;
	}
	else
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "; %s=%f", key, val);
}

inline void CookieField::FixFiled(const char *key, s64 val)
{
	if (_first)
	{
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "%s=%lld", key, val);
		_first = false;
	}
	else
		_offSet += SafeSprintf(_buff + _offSet, _size - _offSet, "; %s=%lld", key, val);
}

#endif //__HTTPMGR_H__

