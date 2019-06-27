/*
 * File:	TextCommand.cpp
 * Author:	xuping
 * 
 * Created On 2019/6/26 14:54:57
 */

#include "TextCommand.h"
TextCommand * TextCommand::s_self = nullptr;
IKernel * TextCommand::s_kernel = nullptr;
CommandHandlerMap TextCommand::s_handlerMap;
bool TextCommand::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    return true;
}

bool TextCommand::Launched(IKernel *kernel)
{
	REG_TEXT_COMMAND(s_self, TestCmmand);
	REG_TEXT_COMMAND(s_self, TestCommand1);
	REG_TEXT_COMMAND(s_self, TestCommand2);

	ExecCommand(s_kernel, "TestCmmand", "echo");
	ExecCommand(s_kernel, "TestCommand1", "32 echo");
	ExecCommand(s_kernel, "TestCommand2", "32 echo");
    return true;
}



bool TextCommand::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

void TextCommand::RegCommandInner(const char *command, ITextCommandHandler *handler)
{
	s_handlerMap.emplace(command, handler);
}

s32 TextCommand::TestCmmand(IKernel *kernel, const char *content)
{
	printf(content);
	return 0;
}

s32 TextCommand::TestCommand1(IKernel *kernel, s32 id, const char *content)
{
	return 0;
}

s32 TextCommand::TestCommand2(IKernel *kernel)
{
	return 0;
}

void TextCommand::ExecCommand(IKernel *kernel, const char *command, const char *args)
{
	s32 len = strlen(args);
	char *tmp = (char*)malloc(len + 1);
	memcpy(tmp, args, len);
	tmp[len] = 0;
	auto iter = s_handlerMap.find(command);
	if (iter != s_handlerMap.end())
	{
		iter->second->DealCommand(kernel, tmp);
	}
}