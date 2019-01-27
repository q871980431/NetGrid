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
typedef std::map<MemeoryFullName, MemeoryDes *> MemeoryMap;
typedef std::map<MemeoryFullName, MemberProperty> PropertyMap;
struct ObjFileSatus 
{
    tools::FileName name;
    tools::FileName dependence;
};
typedef std::map<tools::FileName, tools::FileName> DependenceMap;
typedef std::map<s32, s32>  TypeMap;
class IXmlObject;
class ObjectMgr : public IObjectMgr
{
public:
    virtual ~ObjectMgr(){};

    virtual bool Initialize(IKernel *kernel);
    virtual bool Launched(IKernel *kernel);
    virtual bool Destroy(IKernel *kernel);    

	virtual IObject * CreateObject(const char *fullName);
public:
    virtual s32  GetAttrInt32(void *object, const MemberProperty *member);
    virtual void SetAttrInt32(void *object, const MemberProperty *member, s32 val);
public:
	void    *	 FindChildObj(void *object, const MemberProperty *member);
public:
	void	*	 FindTable(void *object, const MemberProperty *member);
	s32			 GetRowCount(void *table);
	void	*	 CreateRow(void *table);
public:
    static void *CreateObj(const char *fullName);
    static const MemberProperty *GetMemberProperty(const char * fullName);
    static void ReleaseObj(void *object);

	static IObject * InnerCreateObject(const char *fullName);
	static void InnerReleaseObject(IObject *object);
private: 
    static bool LoadObjDir(const char *path);
    static bool LoadDirFiles(const char *path, const char *nameSpace, tools::DirInfo *dirInfo);
    static bool LoadObjFiles(const char *path, const char *dirname, tools::FileMap *files);
    static MemeoryDes * LoadObjFile(const char *path, const char *fileName, const char *nameSpace, tools::FileMap *files, DependenceMap &dependence);
    static MemeoryDes * LoadObjTable(const char *path, const char *nameSpace, tools::FileMap *files, DependenceMap &dependence, IXmlObject *member);
    static void BuildMemberDes(const char *path, const char *fileName, const char *nameSpace, tools::FileMap *files, DependenceMap &dependence, IXmlObject *member, MemeoryDes *memeory, const char *host);
    static bool BuildDependence(const char *file, const char *dependenceFile, DependenceMap &dependence);
    static bool CancelDependence(const char *file, const char *dependenceFile, DependenceMap &dependence);

private:
    static void InitTypeMap();
    static void InitType(const char *type, s8 val, s16 size);

private:
	void TestObject();

private:
    static ObjectMgr     * s_self;
    static IKernel  * s_kernel;

    static TypeMap          s_typeMap;
    static MemeoryMap       s_memeoryMap;
    static PropertyMap      s_propertyMap;
};
#endif