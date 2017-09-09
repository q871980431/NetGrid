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

class Harbor : public IHarbor
{
public:
    virtual ~Harbor(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);

    virtual void AddListener(INodeListener *listener);
    virtual void RegisterMessage(s32 messageId, CALL_BACK fun);
    virtual void Sendbuff(s32 type, s32 nodeId, const char *buff, s32 len);
    virtual void SendMessage(s32 type, s32 nodeId, s32 messageId, const char *buff, s32 len);
public:
    void OnOpen(HarborSession *session);
    void OnRecv(HarborSession *session, s32 messageId, const char *buff, s32 len);
    void OnClose(HarborSession *session);
private:
    inline s64 GetChannel(s32 type, s32 id){ s64 tmp = type;  return tmp << 32 & id; };
    inline s32 GetBuffMapKey(s16 typeA, s16 typeB){ s32 tmp = typeA; return tmp << 16 & typeB; };
    static bool LoadConfigFile(const char *path);
    static s16  GetNodeTypeByName(const char *name);
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
};

class HarborSession : public core::IMsgSession
{
public:
    HarborSession(Harbor *harbor) :_harbor(harbor), _type(0), _nodeId(0), _connection(nullptr){};
    virtual void  SetConnection(IMsgConnection *connection){ _connection = connection; };
    virtual void  OnEstablish(){ _harbor->OnOpen(this); };
    virtual void  OnTerminate(){ _harbor->OnClose(this); };
    virtual void  OnError(s32 moduleErr, s32 sysErr){};
    virtual void  OnRecv(s32 messageId, const char *buff, s32 len);
    inline void SetNodeInfo(s32 type, s32 nodeId){ _type = type; _nodeId = nodeId; };
    inline void SendBuff(const char *buff, s32 len){ _connection->SendBuff(buff, len); };
    inline s32 GetType(){ return _type; };
    inline s32 GetID(){ return _nodeId; };
    inline const char *GetRemoteIP(){ return _connection->GetRemoteIP(); };
private:
    Harbor  * _harbor;
    IMsgConnection  *_connection;
    s32       _type;
    s32       _nodeId;
};

#endif