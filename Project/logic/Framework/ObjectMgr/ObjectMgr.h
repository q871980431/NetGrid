/*
 * File:	ObjectMgr.h
 * Author:	xuping
 * 
 * Created On 2017/9/28 16:21:23
 */

#ifndef __ObjectMgr_h__
#define __ObjectMgr_h__
#include "IObjectMgr.h"
#include "TString.h"
#include "ObjectDes.h"
#include "Tools_file.h"
#include <map>
#include <vector>
typedef tlib::TString<512>    MemeoryFullName;
typedef std::map<MemeoryFullName, ObjectDes *> MemeoryMap;
typedef std::map<MemeoryFullName, MemberProperty *> PropertyMap;
struct ObjFileSatus 
{
    tools::FileName name;
    tools::FileName dependence;
};
const s32 LOCAL_OBJ_MAX = 100000;
const s32 ID_QUERY_SIZE = 100;
struct ObjectGuidInfo 
{
	s32 localId;
	s64 guidMin;
	s64 guidMax;
};

typedef std::map<tools::FileName, tools::FileName> DependenceMap;
typedef std::map<s32, s32>  TypeMap;
typedef std::unordered_map<s64, IObject *> ObjectMap;
class IXmlObject;
class ObjectMgr : public IObjectMgr
{
public:
    virtual ~ObjectMgr(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);    

	virtual void SetGUIDAllocater(const GUID_ALLOCATER_CB &cbFun);
	virtual IObject * CreateObject(const char *fullName, ID_TYPE type = ID_TYPE_GUID, s64 id = 0);
	virtual void ReleaseObject(s64 id);

public:
    static void *CreateObj(const char *fullName);
    static const MemberProperty *GetMemberProperty(const char * fullName);
    static void ReleaseObj(void *object);
	static IObject * InnerCreateObject(const char *fullName, s64 id = 0);
	static void InnerReleaseObject(IObject *object);

private: 
    static bool LoadObjDir(const char *path);
    static bool LoadDirFiles(const char *path, const char *nameSpace, tools::DirInfo *dirInfo);
    static bool LoadObjFiles(const char *path, const char *dirname, tools::FileMap *files);
    static ObjectDes * LoadObjFile(const char *path, const char *fileName, const char *nameSpace, tools::FileMap *files, DependenceMap &dependence);
    static void BuildMemberDes(const char *path, const char *fileName, const char *nameSpace, tools::FileMap *files, DependenceMap &dependence, IXmlObject *member, ObjectDes *memeory, const char *host);
    static bool BuildDependence(const char *file, const char *dependenceFile, DependenceMap &dependence);
    static bool CancelDependence(const char *file, const char *dependenceFile, DependenceMap &dependence);

private:
    static void InitTypeMap();
    static void InitType(const char *type, s8 val, s16 size);
	static s64  GetNewGUID(bool local);
private:
	void TestObject(IKernel *kernel);

private:
    static ObjectMgr		* s_self;
    static IKernel			* s_kernel;

    static TypeMap          s_typeMap;
    static MemeoryMap       s_memeoryMap;
    static PropertyMap      s_propertyMap;
	static GUID_ALLOCATER_CB	s_cbFun;
	static ObjectMap		s_objectMap;
	static ObjectGuidInfo	s_guidInfo;
};
#endif