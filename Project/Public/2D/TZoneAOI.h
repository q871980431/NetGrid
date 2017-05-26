#ifndef __TZone_AOI_h__
#define __TZone_AOI_h__
#include "Vector.h"
#include <list>
#include "TList.h"
#include "TDynPool.h"
#include <unordered_map>
#include "StopWatch.h"
#include <algorithm>  
#include<iterator>
#include <set>
using namespace VectorDefine;

template<typename Identity>
class TRDCAoi
{
    enum 
    {
        VECTOR_NUM = 2,
        OBJ_TYPE_NONE = 0,
        OBJ_TYPE_RM = 1,
        MAX_CHECK_NUM = 128,
        FORCE_COMP_NUM = 16,
        FORMAT_NUM = 5,
    };
    typedef std::list<Identity> IdLists;
    struct ObjNode;
    struct BoundaryVector
    {
        Vector3D    _vect;
        bool        _start;
        ObjNode *   _parent;
        s64         _distance;

        BoundaryVector(Vector3D &vec) :_vect(vec){};
    };

    struct ObjNode
    {
        s8  _type;
        Identity _id;
        Vector3D _pos;
        s64 _distance;
        BoundaryVector _left;
        BoundaryVector _right;
        IdLists  _last;
        IdLists  _now;
        IdLists  _add;
        IdLists  _rm;

        ObjNode(Identity id, Vector3D &pos, Vector3D& boundary) :_id(id), _pos(pos), _left(pos), _right(pos)
        {
            u8 num = boundary.Dimension();
            for (u8 i = 0; i < num; i++)
                _left._vect[i] -= boundary[i];
            for (u8 i = 0; i < num; i++)
                _right._vect[i] += boundary[i];

            _left._start = true;
            _left._parent = this;
            _right._start = false;
            _right._parent = this;

        }

        void Update(Vector3D &pos, Vector3D& boundary)
        {
            _pos = pos;
            _left._vect = pos;
            _right._vect = pos;

            u8 num = boundary.Dimension();
            for (u8 i = 0; i < num; i++)
                _left._vect[i] -= boundary[i];
            for (u8 i = 0; i < num; i++)
                _right._vect[i] += boundary[i];
        }

        bool IsContain(Vector3D &pos)
        {
            u8 num = pos.Dimension();
            for (u8 i = 0; i < num; i++)
            {
                if (pos[i] < _left._vect[i] || pos[i] > _right._vect[i])
                    return false;
            }
            return true;
        }

        bool IsContain(u8 n, s32 num)
        {
            return num < _left._vect[n] || num > _right._vect[n] ? false : true;
        }

    };

    struct SearchInfo
    {
        bool _mark[VECTOR_NUM];
        u8  _searchNum;

        bool IsSearchEnd()
        {
            for (u8 i = 0; i < VECTOR_NUM; i++)
            {
                if (!_mark[i])
                    return false;
            }

            return true;
        }

        void GetNextSearchInfo(SearchInfo &info)
        {
            for (u8 i = 0; i < VECTOR_NUM; i++)
            {
                info._mark[i] = this->_mark[i];
            }
            info._searchNum = _searchNum;
            info._mark[info._searchNum] = true;

            info._searchNum++;
            if (info._searchNum == VECTOR_NUM)
                info._searchNum = 0;
        }

        void GetNewSearchInfo(SearchInfo &info)
        {
            for (u8 i = 0; i < VECTOR_NUM; i++)
            {
                info._mark[i] = false;
            }
            info._searchNum = _searchNum;
            info._mark[info._searchNum] = true;

            info._searchNum++;
            if (info._searchNum == VECTOR_NUM)
                info._searchNum = 0;
        }
    };


    typedef TListNode<ObjNode>              ObjNodeList;
    typedef tlib::TDynPool<ObjNode>         ObjPool;
    typedef std::list<BoundaryVector *>     BoundaryList;
    typedef tlib::TDynPool<BoundaryVector>  BoundaryListPool;
    typedef std::list<ObjNode *>            ObjList;
    typedef tlib::TDynPool<ObjList>         ObjListPool;
    typedef std::unordered_map<s64, ObjNode *>  ObjMap;
    typedef bool(*BoundaryComp)(BoundaryVector * const &left, BoundaryVector * const &right);
    typedef void(*EachBack)(Identity &objA, IdLists *add, IdLists *remove);
    typedef bool(*SearchCallBack)(Identity &objA, s32 num, Identity &objB);

public:
    TRDCAoi():_eachFun(nullptr){};
    ~TRDCAoi(){};
    void SetEachBackFun(EachBack fun){ _eachFun = fun; };
    void SetBoundary(Vector3D &boundary){ _boundVector = boundary; };
    void SetZeroVecter(Vector3D &zero){ _zeroVector = zero; };
public:
    void Enter(Identity id, Vector3D &pos)
    {
        ObjNode *objNode = CREAT_FROM_POOL(_objPool, id, pos, _boundVector);
        _objMap.insert(std::make_pair(id, objNode));

    };
    void Leave(Identity id)
    {
        auto iter = _objMap.find(id);
        if (iter != _objMap.end())
        {
            //Do Leave

            _objPool.Recover(iter->second);
            _objMap.erase(iter);
        }
    };
    void Update(Identity id, Vector3D &pos)
    {
        auto iter = _objMap.find(id);
        if (iter != _objMap.end())
            iter->second->Update(pos, _boundVector);
    };
    void Flush(const SearchCallBack &fun ){
        if (fun == nullptr)
            return;
        PTF("*****************Exec Fulsh***************");
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
        RDClustering(&_boundaryList, &info, fun);

        PTF("Flush Time:%lld ms", watch.Interval());
    };

    void EachObj(){
        for (auto iter = _objMap.begin(); iter != _objMap.end(); iter++)
            _eachFun(iter->second->_id, iter->second->_add, iter->second->_rm);
    }
    void EchoInterest(s64 id){
        auto iter = _objMap.find(id);
        ObjNode *obj = nullptr;
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
    };
    void Clear()
    {
        for (auto iter = _objMap.begin(); iter != _objMap.end(); iter++)
            _objPool.Recover(iter->second);
        _objMap.clear();
    };
protected:
    void RDClustering(BoundaryList *boundaryList, SearchInfo *info, const SearchCallBack &fun){
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
                    DoCompare(objs, info->_searchNum, fun);
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

                        RDClustering(&tmp, &searchInfo, fun);
                    }
                    else
                    {
                        DoCompare(objs, info->_searchNum, fun);
                    }
                }
                startIter = tmpIter;
                objs.clear();
            }

        }
    };
    void DoCompare(ObjList &objs, u8 searchN, const SearchCallBack &fun){
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
                            if (!fun((*iter)->_id, num, (*forwardIter)->_id))
                                break;
                            (*iter)->_now.push_back((*forwardIter)->_id);
                            num++;
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

                if (backIter != objStart)
                {
                    backIter--;
                    if ((*iter)->IsContain((*backIter)->_pos))
                    {
                        if (fun != nullptr)
                        {
                            if (!fun((*iter)->_id, num, (*backIter)->_id))
                                break;
                            (*iter)->_now.push_back((*backIter)->_id);
                            num++;
                        }
                        else
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
            }
            (*iter)->_now.sort();

            tools::Difference((*iter)->_last.begin(), (*iter)->_last.end(), (*iter)->_now.begin(), (*iter)->_now.end(), std::insert_iterator<IdLists >((*iter)->_rm, (*iter)->_rm.begin()), std::insert_iterator<IdLists >((*iter)->_add, (*iter)->_add.begin()));
        }
    };
private:
    ObjPool         _objPool;
    ObjMap          _objMap;
    BoundaryList    _boundaryList;
    Vector3D        _boundVector;
    Vector3D        _zeroVector;
    EachBack        _eachFun;
};
#endif