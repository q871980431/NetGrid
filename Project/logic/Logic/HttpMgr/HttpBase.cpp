#include "HttpBase.h"
size_t WriteData(void * buffer, size_t size, size_t nmemb, void * param) {
	HttpBase * command = (HttpBase*)param;
	ASSERT(command, "wtf");

	if (nullptr == buffer)
		return -1;

	command->Append(buffer, (s32)size * (s32)nmemb);
	return nmemb;
}

bool HttpBase::OnExecute(IKernel * kernel, s32 queueId, s32 threadIdx) {
	ASSERT(_type != HT_NONE, "http request is not initialize");
	curl_easy_reset(_curl);
	curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 10);
	curl_easy_setopt(_curl, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(_curl, CURLOPT_URL, _url);
	curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, WriteData);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, (void*)this);

	struct curl_slist *list = curl_slist_append(nullptr, "Expect:");
	curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, list);
	if (_cookie[0] != 0)
		curl_easy_setopt(_curl, CURLOPT_COOKIE, _cookie);

	if (_type == HT_POST || _type == HT_PUT || _type == HT_DEL) {
		if (_type == HT_POST)
			curl_easy_setopt(_curl, CURLOPT_POST, 1);
		else if (_type == HT_PUT)
			curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
		else if (_type == HT_DEL)
			curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "DELETE");

		curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _field);
	}

	_errCode = curl_easy_perform(_curl);
	curl_slist_free_all(list);
	return _errCode == CURLE_OK;
}

void HttpBase::OnSuccess(IKernel * kernel) {
	_handler->OnSuccess(kernel, _content.data(), _content.size());
}

void HttpBase::OnFailed(IKernel * kernel, bool nonviolent) {
	_handler->OnFail(kernel, _errCode);
}

void HttpBase::OnRelease(IKernel * kernel)
{
	_handler->SetBase(nullptr);
	_handler->OnRelease();
	_handler = nullptr;
	DEL this;
}

void HttpBase::Post(IHttpHandler * handler, bool https)
{
	_https = https;
	_type = HT_POST;
	handler->SetBase(this);
	_handler = handler;
}

void HttpBase::Get(IHttpHandler * handler, bool https)
{
	_type = HT_GET;
	_https = https;
	handler->SetBase(this);
	_handler = handler;
}

void HttpBase::Put(IHttpHandler *handler, const char *url, const char *field)
{
	SafeSprintf(_url, sizeof(_url), "%s", url);
	SafeSprintf(_field, sizeof(_field), "%s", field);
	_type = HT_PUT;
	handler->SetBase(this);
	_handler = handler;
}

void HttpBase::Del(IHttpHandler *handler, const char *url, const char *field)
{
	SafeSprintf(_url, sizeof(_url), "%s", url);
	SafeSprintf(_field, sizeof(_field), "%s", field);
	_type = HT_DEL;
	handler->SetBase(this);
	_handler = handler;
}

void HttpBase::Append(const void * context, const s32 size) {
	_content.append((const char*)context, size);
}
