/*
 * File:	Cluster.h
 * Author:	xuping
 * 
 * Created On 2019/6/17 20:00:12
 */

#ifndef __Cluster_h__
#define __Cluster_h__
#include "ICluster.h"
class Cluster : public ICluster
{
public:
    virtual ~Cluster(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);
protected:
private:
    static Cluster     * s_self;
    static IKernel  * s_kernel;
};
#endif