#ifndef __HTTPBASE_H__
#define __HTTPBASE_H__
#include "IHttpMgr.h"
#include "curl.h"

#define MAX_FIELD 256
#define MAX_URL (256+(MAX_FIELD))
#define DEFAULT_CONTENT_SIZE 256

class HttpBase : public IHttpBase, public IAsyncHandler {
	enum {
		HT_NONE = 0,
		HT_POST,
		HT_GET,
		HT_PUT,
		HT_DEL,
	};
public:
	HttpBase(CURL * curl)
		: _curl(curl)
		, _https(false)
		, _handler(nullptr)
		, _type(HT_NONE)
		, _errCode(0)
		{
		_content.reserve(DEFAULT_CONTENT_SIZE);
		_cookie[0] = 0;
		_field[0] = 0;
		_url[0] = 0;
	}

	virtual ~HttpBase() {}
	virtual bool OnExecute(IKernel * kernel, s32 queueId, s32 threadIdx);
	virtual void OnSuccess(IKernel * kernel);
	virtual void OnFailed(IKernel * kernel, bool nonviolent);
	virtual void OnRelease(IKernel * kernel);

	void Get(IHttpHandler * handler, bool https);
	void Post(IHttpHandler * handler, bool https);
	void Put(IHttpHandler *handler, const char *url, const char *field);
	void Del(IHttpHandler *handler, const char *url, const char *field);

	inline void SetUrl(const char * url) { SafeSprintf(_url, sizeof(_url), "%s", url); }

	inline void SetErrCode(const s32 errCode) { _errCode = errCode; }
	void Append(const void * context, const s32 size);

	inline char *GetUrlHead() { return _url; };
	inline char *GetFieldHead() { return _field; };
	inline char *GetCookieHead() { return _cookie; };

private:
	CURL * _curl;
	char _url[MAX_URL];
	char _field[MAX_FIELD];
	char _cookie[MAX_FIELD];
	bool _https;
	IHttpHandler * _handler;
	s32 _type;

	s32 _errCode;
	std::string _content;
};

#endif //__HTTPBASE_H__
