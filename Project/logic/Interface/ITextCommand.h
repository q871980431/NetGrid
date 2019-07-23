/*
 * File:	ITextCommand.h
 * Author:	xuping
 * 
 * Created On 2019/6/26 14:54:57
 */

#ifndef  __ITextCommand_h__
#define __ITextCommand_h__
#include "IModule.h"
#include "Tools.h"

class ITextCommandHandler
{
public:
	virtual s32 DealCommand(IKernel *kernel, char *content) = 0;
};

const static s32 TEXT_COMMAND_ARGS_ERROR = -1;

typedef char * StringType;
typedef const char * ConstStringType;

inline void Next(char *param, StringType &next)
{
	next = strstr(param, " ");
	if (next)
	{
		*next = 0;
		++next;
	}
}
template<typename T>
inline bool Parse(char *param, T &t, StringType &next)
{
	if (param)
	{
		Next(param, next);
		t = (T)tools::StrToInt32(param);
		return true;
	}

	return false;
}

inline bool Parse(char *param, s64 &t, StringType &next)
{
	if (param)
	{
		Next(param, next);
		t = tools::StrToInt64(param);
		return true;
	}

	return false;
}

inline bool Parse(char *param, float &t, StringType &next)
{
	if (param)
	{
		Next(param, next);
		t = tools::StrToFloat(param);
		return true;
	}

	return false;
}

inline bool Parse(char *param, ConstStringType &t, StringType &next)
{
	if (param)
	{
		Next(param, next);
		t = param;
		return true;
	}

	return false;
}


template<typename... FixArgs>
struct Handler 
{
	template<typename... ParsedArgs>
	struct Dealer
	{
		static s32 invoke(FixArgs... first, char *param, s32(*fn)(FixArgs..., ParsedArgs...), ParsedArgs... args)
		{
			return fn(first..., args...);
		}

		template<typename T, typename... Args>
		static s32 invoke(FixArgs... first, char *param, s32(*fn)(FixArgs..., ParsedArgs..., T, Args ...), ParsedArgs... args)
		{
			char *next = nullptr;
			T t;
			if (!Parse(param, t, next))
				return TEXT_COMMAND_ARGS_ERROR;
			return Dealer<ParsedArgs..., T>::invoke(first..., next, fn, args..., t);
		}

	};


	template<typename... Args>
	static s32 Deal(FixArgs... first, char *param, s32(*fun)(FixArgs..., Args...)) {
		return Dealer<>::invoke(first..., param, fun);
	}

	static s32 Deal(FixArgs... first, char *param, s32(*fun)(FixArgs...))
	{
		return fun(first...);
	}
};

template<typename... Args>
class TextCommandHandler : public ITextCommandHandler
{
public:
	typedef s32(*call_fun)(IKernel *kernel, Args...);
	TextCommandHandler(call_fun f) :_f(f) {};
	virtual ~TextCommandHandler() {};

	virtual s32 DealCommand(IKernel *kernel, char *content)
	{
		return Handler<IKernel*>::Deal(kernel, content, _f);
	}
private:
	call_fun _f;
};

class ITextCommand : public IModule
{
public:
    virtual ~ITextCommand(){};
	
	template<typename... Args>
	void RedCommand(const char *command, s32(*fun)(IKernel *kernel, Args...))
	{
		RegCommandInner(command, NEW TextCommandHandler<Args...>(fun));
	}

protected:
	virtual void RegCommandInner(const char *command, ITextCommandHandler *handler) = 0;
};

#define REG_TEXT_COMMAND(command, func) command->RedCommand(#func, func)

#endif