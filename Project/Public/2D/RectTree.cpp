#include "RectTree.h"
#include "StopWatch.h"
#include "Tools.h"
#include <algorithm>  
#include<iterator>

void RectTree::Enter(s64 id, Vector3D &pos)
{
    VectorNode *objNode = CREAT_FROM_POOL(_objPool, id, pos, _boundVector);

    _searchTree.Insert(objNode->_pos.x, objNode->_posR.x, objNode);
    _objMap.insert(std::make_pair(id, objNode));
}

void RectTree::Leave(s64 id)
{
    auto iter = _objMap.find(id);
    if (iter != _objMap.end())
    {
        //Do Leave
        _searchTree.Remove(iter->second->_pos.x, iter->second->_posR.x, iter->second);

        _objPool.Recover(iter->second);
        _objMap.erase(iter);
    }
}

void RectTree::Clear()
{
    for (auto iter = _objMap.begin(); iter != _objMap.end(); iter++)
        _objPool.Recover(iter->second);
    _objMap.clear();
}

void RectTree::Update(s64 id, Vector3D &pos)
{
    auto iter = _objMap.find(id);
    if (iter != _objMap.end())
    {
        _searchTree.Remove(iter->second->_pos.x, iter->second->_posR.x, iter->second);
        iter->second->Update(pos, _boundVector);
        _searchTree.Insert(iter->second->_pos.x, iter->second->_posR.x, iter->second);    
    }
}
bool QueryResultCallback(RectTree::VectorNode* a_data, void* a_context)
{
    RectTree::VectorNode* obj = (RectTree::VectorNode*)(a_context);
    ASSERT(obj, "obj is null");
    if (obj != a_data )
    {
        obj->_now.push_back(a_data->_id);
        if (obj->_now.size() > 128)
            return false;
    }

    return true;
}
void RectTree::Flush()
{
    PTF("*****************Exec Fulsh***************");
    StopWatch watch;
    ObjMap::iterator iter = _objMap.begin();
    ObjMap::iterator endIter = _objMap.end();
    VectorNode *obj = nullptr;
    for (; iter != endIter; iter++)
    {
        obj = iter->second;
        obj->_last.clear();
        obj->_add.clear();
        obj->_rm.clear();

        obj->_last.swap(obj->_now);
        //_searchTree.Search(iter->second->_pos.x, iter->second->_pos.x, QueryResultCallback, NULL);
        _searchTree.Search(obj->_left.x, obj->_right.x, QueryResultCallback, obj);
        obj->_now.sort();

        tools::Difference(obj->_last.begin(), obj->_last.end(), obj->_now.begin(),
        obj->_now.end(), std::insert_iterator<IdLists >(obj->_rm, obj->_rm.begin()), 
        std::insert_iterator<IdLists >(obj->_add, obj->_add.begin()));
    }

    PTF("Flush Time:%lld ms", watch.Interval());
}

void RectTree::EchoInterest(s64 id)
{
    auto iter = _objMap.find(id);
    VectorNode *obj = nullptr;
    s32 mark = 0;
    if (iter != _objMap.end())
    {
        obj = iter->second;
        PTF("Obj %lld:\n Last Interest ID:\n", id);
        for (auto tmp = obj->_last.begin(); tmp != obj->_last.end(); tmp++)
        {
            printf("     %lld", *tmp);
            if (mark > FORMAT_NUM)
            {
                printf("\n");
                mark = 0;
            }
            mark++;
        }
        mark = 1;
        printf("\n Now Interest ID:\n");
        for (auto tmp = obj->_now.begin(); tmp != obj->_now.end(); tmp++)
        {
            printf("%-10lld", *tmp);
            if (mark >= FORMAT_NUM)
            {
                printf("\n");
                mark = 0;
            }
            mark++;
        }
        printf("\nSize = %d", obj->_now.size());
    }
}
