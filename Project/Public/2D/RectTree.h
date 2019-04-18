#ifndef __Rect_Tree_h__
#define __Rect_Tree_h__
#include "Vector.h"
#include <list>
#include "TList.h"
#include "TDynPool.h"
#include "RTree.h"
#include <unordered_map>
#include <set>

using namespace VectorDefine;
class RectTree
{
public:
    enum
    {
        MAX_CHECK_NUM = 128,
        FORMAT_NUM = 5,
    };

    typedef std::list<s64> IdLists;

    struct VectorNode
    {
        s8  _type;
        s64 _id;
        IdLists *_members;
        Vector3D _pos;
        Vector3D _posR;
        Vector3D _left;
        Vector3D _right;
        IdLists  _last;
        IdLists  _now;
        IdLists  _add;
        IdLists  _rm;

        VectorNode(s64 id, Vector3D &pos, Vector3D& boundary) :_id(id), _pos(pos), _posR(pos), _left(pos), _right(pos)
        {
            Vector3D one(1);
            _left - boundary;
            _right + boundary;
            _posR + one;
        }

        void Update(Vector3D &pos, Vector3D& boundary)
        {
            Vector3D one(1);
            _pos    = pos;
            _left   = pos;
            _right  = pos;
            _posR   = pos;
            _left   - boundary;
            _right  + boundary;
            _posR   + one;
        }
    };

    typedef tlib::TDynPool<VectorNode>              ObjPool;
    typedef std::unordered_map<s64, VectorNode *>   ObjMap;
    typedef RTree<VectorNode *, s32, 2, s32, 64, 48> SearchTree;

public:

public:

    void Enter(s64 id, Vector3D &pos);
    void Leave(s64 id);
    void Update(s64 id, Vector3D &pos);
    void Flush();
    void Clear();

    void SetBoundary(Vector3D &boundary){ _boundVector = boundary; };
    void EchoInterest(s64);
protected:

private:
    ObjPool         _objPool;
    ObjMap          _objMap;
    Vector3D        _boundVector;
    SearchTree      _searchTree;
};

#endif