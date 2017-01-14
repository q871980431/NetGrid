#ifndef __Zone_h__
#define __Zone_h__
#include "MultiSys.h"
#include <set>
#include "IKernel.h"

#define TASSERT ASSERT
typedef s32 GridID_T;
struct WORLD_POS
{
    s32	 x;
    s32	 z;
};
struct VRECT
{
    s32 startX;
    s32 startZ;
    s32 endX;
    s32 endZ;
    VRECT()
    {
        startX = 0;
        startZ = 0;
        endX = 0;
        endZ = 0;
    };
    bool IsContain(s32 x, s32 z)const
    {
        return !(x < startX || x > endX || z < startZ || z > endZ);
    }
};

template< typename T >
class Zone
{
public:
    typedef std::function< bool( const T &data)>ElementCallBackFun;
private:
    class Grid
    {
    public:
        Grid(){};
        ~Grid(){};

        void EnterGrid(const T &data){ _data.insert(data); };
        void LeaveGrid(const T &data){
            auto iter = _data.find(data);
            if (iter != _data.end())
                _data.erase(iter);
        };
        bool TraversingCallAll( const ElementCallBackFun &fun)
        {
            for (auto iter = _data.begin(); iter != _data.end(); iter++)
            if (fun(*iter))return true;
            return false;
        };
    private:
        std::set<T> _data;
    };
    enum 
    {
        INVALID_ID = -1,
    };
public:
    Zone(const WORLD_POS &leftUpPos, const WORLD_POS &rightDownPos, s32 gridLen)
    {
        TASSERT(leftUpPos.x < rightDownPos.x, "x error");
        TASSERT(leftUpPos.z < rightDownPos.z, "z error");
        _leftUpPos = leftUpPos;
        _gridLen = gridLen;
        s32 lenX = rightDownPos.x - leftUpPos.x;
        s32 lenZ = rightDownPos.z - leftUpPos.z;
        _w = lenX / gridLen;
        _h = lenZ / gridLen;
        if (lenX % gridLen > 0)
            ++_w;
        if (lenZ % gridLen > 0)
            ++_h;
        _size = _w * _h;
        _grids = new Grid[_size];
        TASSERT(_grids, "grids is null");
    };
    ~Zone()
    {
        SAFE_DELETE_ARRAY(_grids);
    }

public:
    GridID_T CalcZoneID(WORLD_POS pos)
    {
        if (pos.x < _leftUpPos.x || pos.z < _leftUpPos.z)
        {
            TASSERT(false, "pos error, pos.x = %d, pos.z = %d", pos.x, pos.z);
            return INVALID_ID;
        }
        pos.x -= _leftUpPos.x;
        pos.z -= _leftUpPos.z;
        s32 nX = pos.x / _gridLen;
        s32 nZ = pos.z / _gridLen;

        GridID_T ret = (nX + nZ*_w);
        TASSERT(ret < _size, "grid id error");
        return ret;
    };
    s32 CalcGridSize(s32 distance){ return distance / _gridLen + 1; };
    bool EnterZone(const T &id, GridID_T gridid)
    {
        if (CheckGridID(gridid))
        {
            (_grids + gridid)->EnterGrid(id);
            return true;
        }
        return false;
    };

    bool LeaveZone(const T &id, GridID_T gridid)
    {
        if (CheckGridID(gridid))
        {
            (_grids + gridid)->LeaveGrid(id);
            return true;
        }
        return false;
    };

    void GetAllIDsByRadius(GridID_T gridID, s32 gridRadius, const ElementCallBackFun &fun)
    {
        if ( fun == nullptr )
        {
            TASSERT(false, "fun is null");
            return;
        }
        if (!CheckGridID(gridID))
        {
            TASSERT(false, "gridid error, gridid = %d", gridID);
            return;
        }
        s32 x = gridID % _w;
        s32 z = gridID / _w;
        s32 startX = 0;
        s32 startZ = 0;
        s32 endX = 0;
        s32 endZ = 0;
        startX = (x - gridRadius > 0) ? (x - gridRadius) : 0;
        startZ = (z - gridRadius > 0) ? (z - gridRadius) : 0;
        endX = (x + gridRadius >= _w) ? _w - 1 : x + gridRadius;
        endZ = (z + gridRadius >= _h) ? _h - 1 : z + gridRadius;

        for (z = startZ; z <= endZ; z++)
        {
            for (x = startX; x <= endX; x++)
            {
                GridID_T temp = x + z * _w;
                TASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
                if((_grids + temp)->TraversingCallAll( fun)) return;
            }
        }
    };
    void GetIDsByRadiusFromInner(GridID_T gridID, s32 gridRadius, const ElementCallBackFun &fun)
    {
        if ( fun == nullptr)
        {
            TASSERT(false, "fun is null");
            return;
        }
        if (!CheckGridID(gridID))
        {
            TASSERT(false, "gridid error, gridid = %d", gridID);
            return;
        }
        s32 x = gridID % _w;
        s32 z = gridID / _w;
        s32 startX = 0;
        s32 startZ = 0;
        s32 endX = 0;
        s32 endZ = 0;
        for (s32 i = 0; i <= gridRadius; i++)
        {
            startX = x - i;
            startZ = z - i;
            endX = x + i;
            endZ = z + i;
            for (s32 tempZ = startZ; tempZ <= endZ; tempZ++)
            {
                if (CheckZ(tempZ))
                {
                    if (tempZ == startZ || tempZ == endZ)
                    {
                        for (s32 tempX = startX; tempX <= endX; tempX++)
                        {
                            if (CheckX(tempX))
                            {
                                GridID_T temp = tempX + tempZ * _w;
                                TASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
                                if ((_grids + temp)->TraversingCallAll(fun)) return;
                            }
                        }
                    }
                    else
                    {
                        if (CheckX(startX))
                        {
                            GridID_T temp = startX + tempZ * _w;
                            TASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
                            if ((_grids + temp)->TraversingCallAll(fun)) return;
                        }
                        if (CheckX(endX))
                        {
                            GridID_T temp = endX + tempZ * _w;
                            TASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
                            if ((_grids + temp)->TraversingCallAll(fun)) return;
                        }
                    }
                }
            }

        }
    };
    void GetASubBIds(GridID_T gridA, GridID_T gridB, s32 radius, const ElementCallBackFun &fun)
    {
        if ( fun == nullptr )
        {
            TASSERT(false, "fun is null");
            return;
        }
        if (!(CheckGridID(gridA) && CheckGridID(gridB)))
        {
            TASSERT(false, "GetZoneByRadius_Sub idzoneA or idzoneB error, A = %d, B = %d", gridA, gridB);
            return;
        }
        VRECT vectA;
        VRECT vectB;
        GetRectInRadius(&vectA, gridA, radius);
        GetRectInRadius(&vectB, gridB, radius);
        for (int nz = vectA.startZ; nz <= vectA.endZ; nz++)
        {
            for (int nx = vectA.startX; nx <= vectA.endX; nx++)
            {
                if (!vectB.IsContain(nx, nz))
                {
                    GridID_T zoneid = nx + nz * _w;
                    TASSERT(zoneid < _size && zoneid >= 0, "temp error, zoneid = %d", zoneid);
                    if ((_grids + zoneid)->TraversingCallAll(fun))return;
                }
            }
        }
    };
protected:
private:
    inline bool CheckGridID(GridID_T gridid){ return (gridid >= 0) && (gridid < _size); };
    inline bool CheckX(s32 x){ return x >= 0 && x < _w; };
    inline bool CheckZ(s32 z){ return z >= 0 && z < _h; };
    void GetRectInRadius(VRECT *rect, GridID_T gridId, s32 nRadius)
    {
        s32 x = gridId % _w;
        s32 z = gridId / _w;
        rect->startX = (x - nRadius > 0) ? (x - nRadius) : 0;
        rect->startZ = (z - nRadius > 0) ? (z - nRadius) : 0;
        rect->endX = (x + nRadius >= _w) ? _w - 1 : x + nRadius;
        rect->endZ = (z + nRadius >= _h) ? _h - 1 : z + nRadius;
    };
private:
    WORLD_POS _leftUpPos;
    s32 _w;
    s32 _h;
    s16 _size;
    s32 _gridLen;
    Grid *_grids;
};

#endif