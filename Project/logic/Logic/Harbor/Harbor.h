/*
 * File:	Harbor.h
 * Author:	XuPing
 * 
 * Created On 2017/9/3 22:51:33
 */

#ifndef __Harbor_h__
#define __Harbor_h__
#include "IHarbor.h"
#include <vector>
#include <unordered_map>
#include "TString.h"
#include "FrameworkDefine.h"

struct MasterInfo 
{
    tlib::TString<IP_LEN> ip;
    s16 port;
};

typedef MasterInfo HostInfo;
struct NodeInfo 
{
    tlib::TString<NODE_NAME_LEN> name;
    s16 type;
};

typedef std::vector<INodeListener *> NodeListeners;
typedef std::unordered_map<s32, CALL_BACK> MsgCallBackMap;
typedef std::unordered_map<s16, NodeInfo> NodeMap;
typedef std::unordered_map<s32, s64>    BufferDesMap;
class HarborSession;
typedef std::unordered_map<s64, HarborSession *> SessionMap;
class Harbor : public IHarbor, public INetTcpListener
{
    enum
    {
        BUFFER_SIZE_K = 1024,
    };
public:
    virtual ~Harbor(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

    virtual void AddListener(INodeListener *listener);
    virtual void RegisterMessage(s32 messageId, CALL_BACK fun);
    virtual void Sendbuff(s32 type, s32 nodeId, const void *buff, s32 len);
    virtual void SendMessage(s32 type, s32 nodeId, s32 messageId, const void *buff, s32 len);
    virtual ITcpSession * CreateSession();
	virtual void OnRelease() {};

public:
    void OnOpen(HarborSession *session);
    void OnRecv(HarborSession *session, s32 messageId, const char *buff, s32 len);
    void OnClose(HarborSession *session);

private:
    static inline s64 GetChannel(s32 type, s32 id){ s64 tmp = type;  return tmp << 32 | id; };
    static inline s32 GetBuffMapKey(s16 typeA, s16 typeB){ s32 tmp = typeA; return tmp << 16 | typeB; };
    static inline s64 GetBuffMapValue(s32 send, s32 recv){ s64 tmp = send; return tmp << 32 | recv; };
    static inline void GetSendAndRecvByValue(s64 val, s32 &send, s32 &recv){ send = val >> 32; recv = val &((s32)-1); };
    static bool LoadConfigFile(const char *path);
    static s16  GetNodeTypeByName(const char *name);
    static void OnRecvMasterConnectHarborMsg(s32 type, s32 nodeId, const char *buff, s32 len);
    static void ConnectHarbor(const char *ip, s16 port);
protected:
private:
    static Harbor     * s_self;
    static IKernel  * s_kernel;

    static NodeListeners    s_listeners;
    static MsgCallBackMap   s_callbacks;
    static s32              s_nodeType;
    static s32              s_nodeId;
    static s16              s_port;
    static s32              s_buffer;
    static MasterInfo       s_master;
    static NodeMap          s_nodes;
    static BufferDesMap     s_buffDes;
    static SessionMap       s_sessions;
};

#endif