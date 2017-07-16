#ifndef __Zone_AOI_h__
#define __Zone_AOI_h__
#include "Vector.h"
#include <list>
#include "TDynPool.h"
#include <unordered_map>
#include <set>
typedef void(*call_back)(s64 id);
using namespace VectorDefine;

typedef std::list<s64> IdLists;

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
    s64 _id;
    Vector3D _pos;
    s64 _distance;
    BoundaryVector _left;
    BoundaryVector _right;
    IdLists  _last;
    IdLists  _now;
    IdLists  _add;
    IdLists  _rm;

    ObjNode(s64 id, Vector3D &pos, Vector3D& boundary) :_id(id), _pos(pos), _left(pos), _right(pos)
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

typedef bool(*BoundaryComp)(BoundaryVector * const &left, BoundaryVector * const &right);

class ZoneAoi
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


    typedef tlib::TDynPool<ObjNode>         ObjPool;
    typedef std::list<BoundaryVector *>     BoundaryList;
    typedef tlib::TDynPool<BoundaryVector>  BoundaryListPool;
    typedef std::list<ObjNode *>            ObjList;
    typedef tlib::TDynPool<ObjList>         ObjListPool;
    typedef std::unordered_map<s64, ObjNode *>  ObjMap;

public:
    ZoneAoi(){};
    ~ZoneAoi(){};
    void SetBoundary(Vector3D &boundary){ _boundVector = boundary; };
    void SetZeroVecter(Vector3D &zero){ _zeroVector = zero; };
public:
    void Enter(s64 id, Vector3D &pos);
    void Leave(s64 id);
    void Update(s64 id, Vector3D &pos);
    void Flush( const call_back &fun);
    void EchoInterest(s64 id);
    void Clear();
protected:
    void RDClustering(BoundaryList *boundaryList,  SearchInfo *info);
    void DoCompare( ObjList &objs, u8 searchN);
    void DoCompareSameBound(ObjList &objs, u8 searchN);
private:
    ObjPool         _objPool;
    ObjMap          _objMap;
    BoundaryList    _boundaryList;
    Vector3D        _boundVector;
    Vector3D        _zeroVector;
};

#endif