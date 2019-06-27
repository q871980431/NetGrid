/*
 * File:	TextCommand.h
 * Author:	xuping
 * 
 * Created On 2019/6/26 14:54:57
 */

#ifndef __TextCommand_h__
#define __TextCommand_h__
#include "ITextCommand.h"
#include <unordered_map>

typedef std::unordered_map<std::string, ITextCommandHandler *> CommandHandlerMap;
class TextCommand : public ITextCommand
{
public:
    virtual ~TextCommand(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

	virtual void RegCommandInner(const char *command, ITextCommandHandler *handler);

private:
	static s32 TestCmmand(IKernel *kernel, const char *content);
	static s32 TestCommand1(IKernel *kernel, s32 id, const char *content);
	static s32 TestCommand2(IKernel *kernel);
	static void ExecCommand(IKernel *kernel, const char *command, const char *args);
protected:
private:
    static TextCommand     * s_self;
    static IKernel  * s_kernel;
	static CommandHandlerMap s_handlerMap;
};
#endif