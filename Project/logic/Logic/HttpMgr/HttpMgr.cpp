#include "HttpMgr.h"
#include "HttpBase.h"
#include "XmlReader.h"
#include "tools.h"
#include <string>

HttpMgr * HttpMgr::s_self = nullptr;
IKernel * HttpMgr::s_kernel = nullptr;

std::vector<CURL *> HttpMgr::s_urls;

bool HttpMgr::Initialize(IKernel * kernel) {
    s_self = this;
    s_kernel = kernel;

	XmlReader reader;
	std::string coreConfigPath = std::string(tools::GetAppPath()) + "/config/server_conf.xml";
	if (!reader.LoadFile(coreConfigPath.c_str())) {
		ASSERT(false, "can't find core file : %s", coreConfigPath.c_str());
		return false;
	}

	const char * name = kernel->GetCmdArg("name");

	s32 threadCount = 4;// reader.Root()["extend"][0][name][0]["async"][0].GetAttributeInt32("thread");
	for (s32 i = 0; i < threadCount; ++i) {
		s_urls.push_back(curl_easy_init());
	}

    return true;
}

bool HttpMgr::Launched(IKernel * kernel) {
    return true;
}

bool HttpMgr::Destroy(IKernel * kernel) {
    DEL this;
    return true;
}

void HttpMgr::Get(const s64 id, IHttpHandler * handler, const char * uri, const FieldFillFunc &reqArgsFun, const FieldFillFunc &cookieArgsFun /* = nullptr */, bool htpps /* = false */)
{
	ASSERT(!handler->GetBase(), "can't push http handler twice");
	HttpBase * base = NEW HttpBase(s_urls[id % s_urls.size()]);
	char *uribuff = base->GetUrlHead();
	s32 pos = SafeSprintf(uribuff, MAX_URL, "%s", uri);
	if (reqArgsFun && pos < MAX_URL)
	{
		uribuff[pos] = '?';
		ReqArgsField reqArgsField(uribuff + pos + 1, (s32)(MAX_URL - pos - 1));
		reqArgsFun(s_kernel, &reqArgsField);
	}
	if (cookieArgsFun)
	{
		char *cookieBuff = base->GetCookieHead();
		CookieField cookieField(cookieBuff, MAX_FIELD);
		cookieArgsFun(s_kernel, &cookieField);
	}
	base->Get(handler, htpps);
	IKernel * kernel = s_kernel;
	START_ASYNC(id, base);
}

void HttpMgr::Get(const s64 id, IHttpHandler * handler, const char * uri, const FieldFillFunc &cookieArgsFun /* = nullptr */, bool htpps /* = false */)
{
	ASSERT(!handler->GetBase(), "can't push http handler twice");
	HttpBase * base = NEW HttpBase(s_urls[id % s_urls.size()]);
	char *uribuff = base->GetUrlHead();
	s32 pos = SafeSprintf(uribuff, MAX_URL, "%s", uri);
	if (cookieArgsFun)
	{
		char *cookieBuff = base->GetCookieHead();
		CookieField cookieField(cookieBuff, MAX_FIELD);
		cookieArgsFun(s_kernel, &cookieField);
	}
	base->Get(handler, htpps);
	IKernel * kernel = s_kernel;
	START_ASYNC(id, base);
}

void HttpMgr::Post(const s64 id, IHttpHandler * handler, const char * url, const FieldFillFunc &reqArgsFun, const FieldFillFunc &cookieArgsFun /* = nullptr */, bool htpps /* = false */)
{
	ASSERT(!handler->GetBase(), "can't push http handler twice");
	HttpBase * base = NEW HttpBase(s_urls[id % s_urls.size()]);
	char *uribuff = base->GetUrlHead();
	s32 pos = SafeSprintf(uribuff, MAX_URL, "%s", url);
	char *fieldHead = base->GetFieldHead();
	if (reqArgsFun)
	{
		ReqArgsField reqArgsField(fieldHead, MAX_FIELD);
		reqArgsFun(s_kernel, &reqArgsField);
	}
	if (cookieArgsFun)
	{
		char *cookieBuff = base->GetCookieHead();
		CookieField cookieField(cookieBuff, MAX_FIELD);
		cookieArgsFun(s_kernel, &cookieField);
	}

	base->Post(handler, htpps);

	IKernel * kernel = s_kernel;
	START_ASYNC(id, base);
}

void HttpMgr::Post(const s64 id, IHttpHandler * handler, const char * url, const char *reqArgsContent, const FieldFillFunc &cookieArgsFun /* = nullptr */, bool htpps /* = false */)
{
	ASSERT(!handler->GetBase(), "can't push http handler twice");
	HttpBase * base = NEW HttpBase(s_urls[id % s_urls.size()]);
	char *uribuff = base->GetUrlHead();
	s32 pos = SafeSprintf(uribuff, MAX_URL, "%s", url);
	char *fieldHead = base->GetFieldHead();
	tools::SafeStrcpy(fieldHead, MAX_FIELD, reqArgsContent, strlen(reqArgsContent));
	if (cookieArgsFun)
	{
		char *cookieBuff = base->GetCookieHead();
		CookieField cookieField(cookieBuff, MAX_FIELD);
		cookieArgsFun(s_kernel, &cookieField);
	}

	base->Post(handler, htpps);

	IKernel * kernel = s_kernel;
	START_ASYNC(id, base);
}

void HttpMgr::Stop(IHttpHandler * handler) {
	ASSERT(handler->GetBase(), "http handler is not start");
	if (handler->GetBase())
		s_kernel->StopAsync((HttpBase*)handler->GetBase());
}
