/*
 * File:	ObjectMgr.cpp
 * Author:	xuping
 * 
 * Created On 2017/9/28 16:21:23
 */

#include "ObjectMgr.h"
#include "Tools.h"
#include "XmlReader.h"
#include "MemberDefine.h"
#include "ObjectMember.h"

ObjectMgr * ObjectMgr::s_self = nullptr;
IKernel * ObjectMgr::s_kernel = nullptr;
MemeoryMap ObjectMgr::s_memeoryMap;
TypeMap     ObjectMgr::s_typeMap;
PropertyMap ObjectMgr::s_propertyMap;

bool ObjectMgr::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
    InitTypeMap();
    char path[MAX_PATH];
    SafeSprintf(path, sizeof(path), "%s/object", s_kernel->GetEnvirPath());
    if (!LoadObjDir(path))
        return false;
       
    return true;
}

bool ObjectMgr::Launched(IKernel *kernel)
{
    void *data = CreateObj("logic::wing");
    const MemberProperty *propWingid = GetMemberProperty("logic::wing::wingid");
    s32 wingId = GetAttrInt32(data, propWingid);
    SetAttrInt32(data, propWingid, 15);
    wingId = GetAttrInt32(data, propWingid);
    ReleaseObj(data);
    ObjectMember::Init();

    return true;
}

bool ObjectMgr::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

s32 ObjectMgr::GetAttrInt32(void *object, const MemberProperty *member)
{
    const MemeoryDes **des = (const MemeoryDes **)((char*)object);
    s32 ret = (*des)->GetAttrT<s32>(object, member);
    return ret;
}

void ObjectMgr::SetAttrInt32(void *object, const MemberProperty *member, s32 val)
{
    const MemeoryDes **des = (const MemeoryDes **)object;  
    (*des)->SetAttrT(object, member, val);
}

void OnSetAttrCallBack(void *object, const MemberProperty *member, void *context, s32 size)
{
    const MemeoryDes **des = (const MemeoryDes **)object;
    ECHO("obj %s, member %s changeing", (*des)->GetMemeoryName(), member->des->name.GetString());
}

void * ObjectMgr::CreateObj(const char *fullName)
{
    auto iter = s_memeoryMap.find(fullName);
    if (iter == s_memeoryMap.end())
    {
        //ASSERT(false, "error");
        return nullptr;
    }
    s32 size = 0;
    void *ret = iter->second->CreateMemeory(size);
    iter->second->RegisterMemberChangeCallBack(ret, ANY, OnSetAttrCallBack, "On SetAttrCallBack");
    return ret;
}

void ObjectMgr::ReleaseObj(void *object)
{
    const MemeoryDes **des = (const MemeoryDes **)((char*)object);
    (*des)->DestoryMemeory(object);
}

const MemberProperty * ObjectMgr::GetMemberProperty(const char * fullName)
{
    auto iter = s_propertyMap.find(fullName);
    if ( iter == s_propertyMap.end())
    {
        //ASSERT(false, "error");
        return nullptr;
    }
    return &iter->second;
}

bool ObjectMgr::LoadObjDir(const char *path)
{
    tools::DirInfo dir;
    tools::FindDirectoryFiles(path, "xml", dir);
    ECHO("load %s dir all xml file", path);
    char tmp[MAX_PATH] = { 0 };
    tools::GetFileNameNotExtension(dir.attr.name.GetString(), tmp);
    return LoadDirFiles(path, init_namespace, &dir);
}

bool ObjectMgr::LoadDirFiles(const char *path, const char *nameSpace, tools::DirInfo *dirInfo)
{
    if (!LoadObjFiles(path, nameSpace, &dirInfo->files))
        return false;
    for (auto dirIter : dirInfo->dirs)
    {
        tools::FileName subPath(path);
        subPath << "/"<< dirIter.attr.name;
        tools::FileName subSpace(nameSpace);
        char tmp[MAX_PATH] = { 0 };
        tools::GetFileNameNotExtension(dirIter.attr.name.GetString(), tmp);
        if (nameSpace[0] != 0)
            subSpace << space_split_name;
        subSpace << tmp;
        if (!LoadDirFiles(subPath.GetString(), subSpace.GetString(), &dirIter))
            return false;
    }

    return true;
}


bool ObjectMgr::LoadObjFiles(const char *path, const char *dirname, tools::FileMap *files)
{
    char spacename[MAX_PATH];
    typedef std::map<tools::FileName, tools::FileName> ParentMap;
    ParentMap parentMap;

    for (auto iter : *files)
    {
        char filePath[MAX_PATH];
        SafeSprintf(filePath, sizeof(filePath), "%s/%s", path, iter.first.GetString());
        if (LoadObjFile(path, iter.first.GetString(), dirname, files, parentMap) == nullptr)
            return false;
    }

    return true;
}

MemeoryDes * ObjectMgr::LoadObjFile(const char *path, const char *fileName, const char *nameSpace, tools::FileMap *files, DependenceMap &dependence)
{
    char filePath[MAX_PATH];
    SafeSprintf(filePath, sizeof(filePath), "%s/%s", path, fileName);
    char simpleName[MAX_PATH] = { 0 };
    tools::GetFileNameNotExtension(fileName, simpleName);

    MemeoryFullName fullName(nameSpace);
    if (nameSpace[0] != 0)
        fullName << space_split_name;
    fullName << simpleName;

    auto iter = s_memeoryMap.find(fullName);
    if (iter != s_memeoryMap.end())
        return iter->second;

    XmlReader readr;
    if (!readr.LoadFile(filePath))
    {
        ASSERT(false, "don't find file:%s", filePath);
        return nullptr;
    }
    const char *parent = readr.Root()->GetAttribute_Str("parent");
    bool isCommon = true;
    if (readr.Root()->HasAttribute("special"))
        isCommon = readr.Root()->GetAttribute_Bool("special") ? false : true;
    MemeoryDes *parentDes = nullptr;
    if (strcmp(parent, "") != 0)
    {
        BuildDependence(fileName, parent, dependence);
        tools::FileName parentFile(parent);
        parentFile << "." << des_extension_name;

        parentDes = LoadObjFile(path, parentFile.GetString(), nameSpace, files, dependence);
        CancelDependence(fileName, parent, dependence);
    }
    MemeoryDes *memeory = NEW MemeoryDes(simpleName, parentDes, isCommon);
    IXmlObject *member = readr.Root()->GetFirstChrild("member");
    BuildMemberDes(path, fileName, nameSpace, files, dependence, member, memeory, fullName.GetString());
    auto ret = s_memeoryMap.insert(std::make_pair(fullName, memeory));
    ASSERT(ret.second, "error");
    memeory->Fix();
    PropertyLists propertyLists;
    memeory->GetProperty(propertyLists);
    for (auto prop : propertyLists)
    {
        MemberName name(fullName.GetString());
        name << mem_split_name << prop.name.GetString();
        prop.name = name;
        auto propRet = s_propertyMap.insert(std::make_pair(prop.name.GetString(), prop));
        ASSERT(propRet.second, "error");
    }

    return memeory;
}

void ObjectMgr::BuildMemberDes(const char *path, const char *fileName, const char *nameSpace, tools::FileMap *files, DependenceMap &dependence, IXmlObject *member, MemeoryDes *memeory, const char *host)
{
    IXmlObject *childMember = member;
    while (childMember != nullptr)
    {
        MemberDes des;
        const char *name = childMember->GetAttribute_Str("name");
        const char *type = childMember->GetAttribute_Str("type");
        des.index = childMember->GetAttribute_S32("index");
        s32 mark = childMember->GetAttribute_S32("mark");

        des.name = name;
        auto typeIter = s_typeMap.find(tools::HashKey(type));
        if (typeIter == s_typeMap.end())
        {
            des.type = DATA_TYPE_OBJ;
            BuildDependence(fileName, type, dependence);
            tools::FileName dependenceFile(type);
            dependenceFile << "." << des_extension_name;

            des.memeoryDes = LoadObjFile(path, dependenceFile.GetString(), nameSpace, files, dependence);
            des.size = des.memeoryDes->GetMemorySize();
            CancelDependence(fileName, type, dependence);
        }
        else
        {
            tools::KEYINT32 key;
            key.val = typeIter->second;
            des.size = key.hVal;
            des.type = key.lVal;
            if (des.type == DATA_TYPE_TABLE)
            {
                MemeoryDes *tableMemeory = NEW MemeoryDes(name, nullptr, true);
                IXmlObject *tableMember = childMember->GetFirstChrild("member");
                tools::FileName childHost(host);
                childHost << mem_split_name << name;
                BuildMemberDes(path, fileName, nameSpace, files, dependence, tableMember, tableMemeory, childHost.GetString());
                des.memeoryDes = tableMemeory;
                auto ret = s_memeoryMap.insert(std::make_pair(childHost.GetString(), tableMemeory));
                ASSERT(ret.second, "error");
                tableMemeory->Fix();
            }
            else
            {
                if (key.hVal == 0)
                    des.size = childMember->GetAttribute_S32("size");
            }

        }
        memeory->AddMember(des);
        childMember = childMember->GetNextSibling();
    }
}



bool ObjectMgr::BuildDependence(const char *file, const char *dependenceFile, DependenceMap &dependence)
{
    auto dependenceIter = dependence.find(file);
    if (dependenceIter != dependence.end())
    {
        ASSERT(false, "%s dependence %s, but %s had dependence %s", file, dependenceFile, dependenceFile, dependenceIter->second.GetString());
        return false;
    }
    auto ret = dependence.insert(std::make_pair(file, dependenceFile));
    ASSERT(ret.second, "error");
    return true;
}

bool ObjectMgr::CancelDependence(const char *file, const char *dependenceFile, DependenceMap &dependence)
{
    auto dependenceIter = dependence.find(file);
    if (dependenceIter == dependence.end())
    {
        ASSERT(false, "don't find %s dependence file", file);
        return false;
    }
    if (dependenceIter->second != dependenceFile)
    {
        ASSERT(false, "not't match file, dependenceFile:%s, find file:%s", dependenceFile, dependenceIter->second.GetString());
        return false;
    }
    dependence.erase(dependenceIter);
    return true;
}

void ObjectMgr::InitTypeMap()
{
    InitType(bool_name, DATA_TYPE_BOOL, sizeof(bool));
    InitType(s8_name, DATA_TYPE_S8, sizeof(s8));
    InitType(s16_name, DATA_TYPE_S16, sizeof(s16));
    InitType(s32_name, DATA_TYPE_S32, sizeof(s32));
    InitType(s64_name, DATA_TYPE_S64, sizeof(s64));
    InitType(float_name, DATA_TYPE_FLOAT, sizeof(float));
    InitType(str_name, DATA_TYPE_STR, 0);
    InitType(binary_name, DATA_TYPE_BINARY, 0);
    InitType(table_name, DATA_TYPE_TABLE, 0);
}

void ObjectMgr::InitType(const char *type, s8 val, s16 size)
{
    tools::KEYINT32 key;
    key.lVal = val;
    key.hVal = size;

    auto ret = s_typeMap.insert(std::make_pair(tools::HashKey(type), key.val));
    ASSERT(ret.second, "error");
}