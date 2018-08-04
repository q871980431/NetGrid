/*
 * File:	LuaTest.h
 * Author:	Xuping
 * 
 * Created On 2018/6/23 0:04:33
 */

#ifndef __LuaTest_h__
#define __LuaTest_h__
#include "ILuaTest.h"

#include <iostream>
template <typename ... T>
void DummyWrapper(T... t) {}

template <class T>
T unpacker(const T& t) {
	std::cout << ',' << t;
	return t;
}

template <typename T, typename... Args>
void write_line(const T& t, const Args& ... data) {
	std::cout << ',';
	DummyWrapper(unpacker(data)...);
	std::cout << '\n';
}

template <typename T>
void unpack(T&& t)
{
	std::cout << std::forward<T>(t) << ' ';
}

template <typename ... Args>
void debugLogImpl(Args&& ... args)
{
	int dummy1[] = { 0, (0, 0) };
	int dummy[] = { 0 , (unpack(args), 0)... };

	int size = sizeof(dummy);
	//int dummy[] = { 0 , (unpack(std::forward<Args>(args)), 0)... };
	std::cout << '\n';
}

template <typename ... Args>
void debugLog(Args&& ... args)
{
	debugLogImpl(std::forward<Args>(args)...);
}

class LuaTest : public ILuaTest
{
public:
    virtual ~LuaTest(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static LuaTest     * s_self;
    static IKernel  * s_kernel;
};
#endif