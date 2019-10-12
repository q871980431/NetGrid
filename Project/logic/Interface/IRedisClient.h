/*
 * File:	IRedisClient.h
 * Author:	xuping
 * 
 * Created On 2019/8/16 14:32:57
 */

#ifndef  __IRedisClient_h__
#define __IRedisClient_h__
#include "IModule.h"
class IRedisClient : public IModule
{
public:
    virtual ~IRedisClient(){};
    
};
#endif