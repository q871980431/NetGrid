#include "ZoneAOI.h"
#include "Tools.h"
#include "StopWatch.h"
#include <algorithm>  
#include<iterator>

bool XBoundaryComp(BoundaryVector* const&left, BoundaryVector* const&right)
{
    return left->_vect.GetVal(X) < right->_vect.GetVal(X);
}

bool ZBoundaryComp(BoundaryVector* const &left, BoundaryVector* const &right)
{
    return left->_vect.GetVal(Z) < right->_vect.GetVal(Z);
}

bool YBoundaryComp(BoundaryVector* const &left, BoundaryVector* const &right)
{
    return left->_vect.GetVal(Y) < right->_vect.GetVal(Y);
}

void ZoneAoi::Enter(s64 id, Vector3D &pos)
{
    ObjNode *objNode = CREAT_FROM_POOL(_objPool, id, pos, _boundVector);
    objNode->_type = OBJ_TYPE_NONE;
    //for (u8 i = 0; i < VECTOR_NUM; i++)
    //{
    //    _boundaryList[i].push_back(&objNode->_left);
    //    _boundaryList[i].push_back(&objNode->_right);
    //}
    _objMap.insert(std::make_pair(id, objNode));
}

void ZoneAoi::Leave(s64 id)
{
    auto iter = _objMap.find(id);
    if (iter != _objMap.end())
    {
        //Do Leave

        iter->second->_type = OBJ_TYPE_RM;
        _objPool.Recover(iter->second);
        _objMap.erase(iter);
    }
}

void ZoneAoi::Clear()
{
    for (auto iter = _objMap.begin(); iter != _objMap.end(); iter++)
        _objPool.Recover(iter->second);
    _objMap.clear();
}

void ZoneAoi::Update(s64 id, Vector3D &pos)
{
    auto iter = _objMap.find(id);
    if (iter != _objMap.end())
        iter->second->Update(pos, _boundVector);
}
void ZoneAoi::Flush( const call_back &fun)
{
    //PTF("*****************Exec Fulsh***************");
    StopWatch watch;
    auto iter = _objMap.begin();
    auto endIter = _objMap.end();
    _boundaryList.clear();

    for (; iter != endIter; iter++)
    {
        iter->second->_last.clear();
        iter->second->_add.clear();
        iter->second->_rm.clear();

        iter->second->_last.swap(iter->second->_now);
        if (iter->second->_type != OBJ_TYPE_RM)
        {
            _boundaryList.push_back(&(iter->second->_left));
            _boundaryList.push_back(&(iter->second->_right));
        }
    }

    SearchInfo info;
    info._searchNum = X;
    for (u8 i = 0; i < VECTOR_NUM; i++)
        info._mark[i] = false;
    RDClustering(&_boundaryList, &info);

    PTF("Flush Time:%lld ms", watch.Interval());
}


void ZoneAoi::RDClustering(BoundaryList *boundaryList, SearchInfo *info)
{
    s32 tmp = info->_searchNum;
    auto f = [tmp](BoundaryVector* const&left, BoundaryVector* const&right)
    {
        return left->_vect.GetVal(tmp) < right->_vect.GetVal(tmp);
    };
    boundaryList->sort(f);

    auto startIter = boundaryList->begin();
    auto endIter = boundaryList->end();
    s32 count = 0;
    ObjList objs;
    for (auto iter = startIter; iter != endIter; iter++)
    {
        if ((*iter)->_start)
        {
            objs.push_back((*iter)->_parent);
            count++;
        }
        else
        {
            count--;
        }

        if (count == 0)
        {
            auto tmpIter = iter;
            tmpIter++;

            if (objs.size() <= FORCE_COMP_NUM)
            {
                DoCompare(objs, info->_searchNum);
            }
            else
            {
                info->_mark[info->_searchNum] = true;
                if (!info->IsSearchEnd())
                {
                    SearchInfo searchInfo;
                    if (tmpIter == endIter)
                        info->GetNextSearchInfo(searchInfo);
                    else
                        info->GetNewSearchInfo(searchInfo);

                    BoundaryList tmp(startIter, tmpIter);

                    RDClustering(&tmp, &searchInfo);
                }
                else
                {
                    DoCompare(objs, info->_searchNum);
                }
            }
            startIter = tmpIter;
            objs.clear();
        }

    }
}

void ZoneAoi::DoCompare(ObjList &objs, u8 searchN)
{
    ObjList::iterator objStart = objs.begin();
    ObjList::iterator objEnd = objs.end();
    ObjList::iterator forwardIter = objStart;
    ObjList::iterator backIter = objStart;

    for (ObjList::iterator iter = objStart; iter != objEnd; iter++)
    {
        forwardIter = iter;
        backIter = iter;
        s32 num = 0;

        while (forwardIter != objEnd || backIter != objStart)
        {
            if (forwardIter != objEnd)
            {
                forwardIter++;
                if (forwardIter != objEnd)
                {
                    if ((*iter)->IsContain((*forwardIter)->_pos))
                    {
                        (*iter)->_now.push_back((*forwardIter)->_id);
                    }
                    else
                    {
                        if (!(*iter)->IsContain(searchN,(*forwardIter)->_pos[searchN]))
                        {
                            forwardIter = objEnd;
                        }
                    }
                }
            }

            if (backIter != objStart)
            {
                backIter--;
                if ((*iter)->IsContain((*backIter)->_pos))
                {
                    (*iter)->_now.push_back((*backIter)->_id);
                }
                else
                {
                    if (!(*iter)->IsContain(searchN, (*backIter)->_pos[searchN]))
                    {
                        backIter = objStart;
                    }

                }
            }

            if ((*iter)->_now.size() >= MAX_CHECK_NUM)
                break;
        }
        (*iter)->_now.sort();

        num = (*iter)->_now.size();
        tools::Difference((*iter)->_last.begin(), (*iter)->_last.end(), (*iter)->_now.begin(), (*iter)->_now.end(), std::insert_iterator<IdLists >((*iter)->_rm, (*iter)->_rm.begin()), std::insert_iterator<IdLists >((*iter)->_add, (*iter)->_add.begin()));
    }
}

void ZoneAoi::DoCompareSameBound(ObjList &objs, u8 searchN)
{
    ObjList::iterator objStart = objs.begin();
    ObjList::iterator objEnd = objs.end();
    ObjList::iterator forwardIter = objStart;
    ObjList::iterator backIter = objStart;

    for (ObjList::iterator iter = objStart; iter != objEnd; iter++)
    {
        forwardIter = iter;
        backIter = iter;
        s32 num = 0;

        while (forwardIter != objEnd )
        {
            if (forwardIter != objEnd)
            {
                forwardIter++;
                if (forwardIter != objEnd)
                {
                    if ((*iter)->IsContain((*forwardIter)->_pos))
                    {
                        (*iter)->_now.push_back((*forwardIter)->_id);
                        (*forwardIter)->_now.push_back((*iter)->_id);
                    }
                    else
                    {
                        if (!(*iter)->IsContain(searchN, (*forwardIter)->_pos[searchN]))
                        {
                            forwardIter = objEnd;
                        }
                    }
                }
            }
            if ((*iter)->_now.size() >= MAX_CHECK_NUM)
                break;
        }
        (*iter)->_now.sort();

        num = (*iter)->_now.size();
        tools::Difference((*iter)->_last.begin(), (*iter)->_last.end(), (*iter)->_now.begin(), (*iter)->_now.end(), std::insert_iterator<IdLists >((*iter)->_rm, (*iter)->_rm.begin()), std::insert_iterator<IdLists >((*iter)->_add, (*iter)->_add.begin()));
    }
}

void ZoneAoi::EchoInterest(s64 id)
{
    auto iter = _objMap.find(id);
    ObjNode *obj = nullptr;
    s32 mark = 0;
    if (iter != _objMap.end())
    {
        obj = iter->second;
        //PTF("Obj %lld:\n Last Interest ID:\n", id);
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

