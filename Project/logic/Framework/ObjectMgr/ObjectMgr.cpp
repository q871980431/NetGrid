/*
 * File:	ObjectMgr.cpp
 * Author:	xuping
 * 
 * Created On 2017/9/28 16:21:23
 */

#include "ObjectMgr.h"
#include "Tools.h"
#include "XmlReader.h"
#include "ObjectMember.h"
#include "MemberDef.h"
#include "CommonObject.h"

ObjectMgr * ObjectMgr::s_self = nullptr;
IKernel * ObjectMgr::s_kernel = nullptr;
MemeoryMap ObjectMgr::s_memeoryMap;
TypeMap     ObjectMgr::s_typeMap;
PropertyMap ObjectMgr::s_propertyMap;
GUID_ALLOCATER_CB ObjectMgr::s_cbFun = nullptr;
ObjectMap   ObjectMgr::s_objectMap;
ObjectGuidInfo ObjectMgr::s_guidInfo;

bool ObjectMgr::Initialize(IKernel *kernel)
{
    s_self = this;
    s_kernel = kernel;
	tools::Zero(s_guidInfo);
    InitTypeMap();
    char path[MAX_PATH];
    SafeSprintf(path, sizeof(path), "%s/object", s_kernel->GetEnvirPath());
    if (!LoadObjDir(path))
        return false;
       
    return true;
}

bool ObjectMgr::Launched(IKernel *kernel)
{
    const MemberProperty *propWingid = GetMemberProperty("logic::wing::wingid");
    ObjectMember::Init();
	TestObject();

    return true;
}

bool ObjectMgr::Destroy(IKernel *kernel)
{
    DEL this;
    return true;
}

void ObjectMgr::SetGUIDAllocater(const GUID_ALLOCATER_CB &cbFun)
{
	s_cbFun = cbFun;
}

IObject * ObjectMgr::CreateObject(const char *fullName, ID_TYPE type /* = ID_TYPE_GUID */, s64 id /* = 0 */)
{
	if (id == 0)
		id = GetNewGUID(type == ID_TYPE_LOCAL);
	if (id == 0)
		return nullptr;

	IObject *object = InnerCreateObject(fullName);
	s_objectMap.emplace(id, object);
	return object;
}

void ObjectMgr::ReleaseObject(s64 id)
{
	auto iter = s_objectMap.find(id);
	if (iter != s_objectMap.end())
	{
		InnerReleaseObject(iter->second);
		s_objectMap.erase(iter);
	}
}

void OnSetAttrCallBack(void *object, const MemberProperty *member, void *context, s32 size)
{
    const ObjectDes **des = (const ObjectDes **)object;
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
    //iter->second->RegisterMemberChangeCallBack(ret, ANY, OnSetAttrCallBack, "On SetAttrCallBack");
    return ret;
}

void ObjectMgr::ReleaseObj(void *object)
{
    const ObjectDes **des = (const ObjectDes **)((char*)object);
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
    return iter->second;
}

IObject * ObjectMgr::InnerCreateObject(const char *fullName, s64 id /* = 0 */)
{
	auto iter = s_memeoryMap.find(fullName);
	if (iter == s_memeoryMap.end())
	{
		//ASSERT(false, "error");
		return nullptr;
	}
	CommonObject *ret = NEW CommonObject(iter->second, id);
	return ret;
}

void ObjectMgr::InnerReleaseObject(IObject *object)
{
	DEL object;
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

ObjectDes * ObjectMgr::LoadObjFile(const char *path, const char *fileName, const char *nameSpace, tools::FileMap *files, DependenceMap &dependence)
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
    ObjectDes *parentDes = nullptr;
    if (strcmp(parent, "") != 0)
    {
        BuildDependence(fileName, parent, dependence);
        tools::FileName parentFile(parent);
        parentFile << "." << des_extension_name;

        parentDes = LoadObjFile(path, parentFile.GetString(), nameSpace, files, dependence);
        CancelDependence(fileName, parent, dependence);
    }
    ObjectDes *memeory = NEW ObjectDes(simpleName, parentDes);
    IXmlObject *member = readr.Root()->GetFirstChrild("member");
    BuildMemberDes(path, fileName, nameSpace, files, dependence, member, memeory, fullName.GetString());
    auto ret = s_memeoryMap.insert(std::make_pair(fullName, memeory));
    ASSERT(ret.second, "error");
    memeory->Fix();
	const auto &propertyMap = memeory->GetPropertyMap();
	for (auto &prop : propertyMap)
	{
		MemberName name(fullName.GetString());
		name << mem_split_name << prop.first.c_str();
		auto propRet = s_propertyMap.insert(std::make_pair(name.GetString(), prop.second));
		ASSERT(propRet.second, "error");

	}
    //PropertyLists propertyLists;
    //memeory->GetProperty(propertyLists);
    //for (auto prop : propertyLists)
    //{
    //    MemberName name(fullName.GetString());
    //    name << mem_split_name << prop.name.GetString();
    //    prop.name = name;
    //    auto propRet = s_propertyMap.insert(std::make_pair(prop.name.GetString(), prop));
    //    ASSERT(propRet.second, "error");
    //}

    return memeory;
}

void ObjectMgr::BuildMemberDes(const char *path, const char *fileName, const char *nameSpace, tools::FileMap *files, DependenceMap &dependence, IXmlObject *member, ObjectDes *memeory, const char *host)
{
    IXmlObject *childMember = member;
    while (childMember != nullptr)
    {
        MemberDes des;
		des.memeoryDes = nullptr;
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
				tools::FileName childHost(host);
				childHost << mem_split_name << name;
				TableDes *tableMemeory = NEW TableDes(childHost.GetString());
				if (childMember->HasAttribute("key"))
				{
					const char *keyName = childMember->GetAttribute_Str("key");
					if (childMember->HasAttribute("des"))
					{
						bool des = childMember->GetAttribute_Bool("des");
						tableMemeory->SetDes(des);
					}
					tableMemeory->SetKeyName(keyName);
				}

                IXmlObject *tableMember = childMember->GetFirstChrild("member");
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

s64 ObjectMgr::GetNewGUID(bool local)
{
	s64 id = 0;
	if (local)
	{
		for (s32 i = 0; i < ID_QUERY_SIZE; i++)
		{
			id = ++s_guidInfo.localId;
			if (s_guidInfo.localId == LOCAL_OBJ_MAX)
				s_guidInfo.localId = 0;

			auto iter = s_objectMap.find(id);
			if (iter == s_objectMap.end())
				break;
		}
	}
	else
	{
		if (s_cbFun != nullptr)
		{
			for (s32 i = 0; i < ID_QUERY_SIZE; i++)
			{
				if (s_guidInfo.guidMin < s_guidInfo.guidMax)
				{
					auto iter = s_objectMap.find(s_guidInfo.guidMin);
					if (iter == s_objectMap.end())
						id = s_guidInfo.guidMin++;
				}
				else
				{
					s32 num = 0;
					s_cbFun(s_guidInfo.guidMin, num);
					s_guidInfo.guidMax = s_guidInfo.guidMin + num;
				}
			}

		}
	}

	return id;
}

void ObjectMgr::TestObject()
{
	IObject *player = InnerCreateObject("logic::player");
	s64 oldLevel = player->GetMemberS64(Logic::Player::lvl);
	player->SetMemberS64(Logic::Player::lvl, oldLevel + 10);
	s64 nowLevel = player->GetMemberS64(Logic::Player::lvl);
	InnerReleaseObject(player);
	player = InnerCreateObject("logic::player");
	oldLevel = player->GetMemberS64(Logic::Player::lvl);
	player->SetMemberS64(Logic::Player::lvl, oldLevel + 10);
	nowLevel = player->GetMemberS64(Logic::Player::lvl);
	ITable *bag = player->GetTable(Logic::Player::bag);
	s32 count = bag->RowCount();
	IRow *row = bag->CreateRow();
	row->SetMemberS32(Logic::Player::Bag::place, 1);
	s32 place = row->GetMemberS32(Logic::Player::Bag::place);
	count = bag->RowCount();
	const char *name = player->GetMemberStr(Logic::Player::name);
	player->SetMemberStr(Logic::Player::name, "test0test1test2test3");
	name = player->GetMemberStr(Logic::Player::name);
	ITable *notic = player->GetTable(Logic::Player::notice);
	s32 noticCount = notic->RowCount();
	IRow *noticRow = nullptr;
	noticRow = notic->AddRowByKey("test2");
	noticRow = notic->AddRowByKey("test1");
	noticRow = notic->AddRowByKey("test3");
	auto fun = [](IRow *row)
	{
		IKernel *kernel = s_kernel;
		const char *content = row->GetMemberStr(Logic::Player::Notice::content);
		DEBUG_LOG("notice, content:%s", content);
	};
	notic->ForEach(fun);
	IRow *findRow = notic->FindRowByKey("test1");
	findRow = notic->FindRowByKey("testt");
	noticRow->SetMemberStr(Logic::Player::Notice::content, "test4");
	noticRow->SetMemberStr(Logic::Player::name, "xuping");
}