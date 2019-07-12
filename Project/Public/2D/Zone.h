#ifndef __Zone_h__
#define __Zone_h__
#include "MultiSys.h"
#include <unordered_set>
typedef s32 GridID_T;

struct WORLD_POS2D
{
    s32	 x;
    s32	 z;
    WORLD_POS2D(s32 _x = 0, s32 _z = 0) :x(_x), z(_z){};
};
template< typename T >
class Zone
{
    struct Rect
    {
        s32 startX;
        s32 startZ;
        s32 endX;
        s32 endZ;
        Rect()
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
        inline bool Valied(){ return (startX != endX && startZ != endZ); };
    };

public:
    typedef std::function< bool( const T &data)>ElementCallBackFun;
    enum
    {
        INVALID_ID = -1,
    };
private:
    class Grid
    {
    public:
        Grid(){};
        ~Grid(){ _data.clear(); };

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
        std::unordered_set<T> _data;
    };
public:
    Zone() :_grids(nullptr)
    {

    };
    ~Zone()
    {
        Clean();
    }

public:

    bool Init(const WORLD_POS2D &leftUpPos, const WORLD_POS2D &rightDownPos, s32 gridLen)
    {

        if ( !(leftUpPos.x < rightDownPos.x && leftUpPos.z < rightDownPos.z) )
        {
            ASSERT(false, "x error");
            ASSERT(false, "z error");
            return false;
        }
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
        s64 count = _w * _h;
        ECHO("*******W=%d, H=%d***********", _w, _h);
        ASSERT(count <= 0xFFFF, "count is big = %ld, w = %d, h = %d", count, _w, _h);
        _size = _w * _h;
        _grids = NEW Grid[_size];
        ASSERT(_grids, "grids is null");
        return true;
    }
    void Clean()
    {
        if (nullptr != _grids)
        {
            DEL[] _grids;
            _grids = nullptr;
        }
    }

    GridID_T CalcZoneID(WORLD_POS2D pos)
    {
        if (pos.x < _leftUpPos.x || pos.z < _leftUpPos.z)
        {
            return INVALID_ID;
        }
        pos.x -= _leftUpPos.x;
        pos.z -= _leftUpPos.z;
        s32 nX = pos.x / _gridLen;
        s32 nZ = pos.z / _gridLen;

        GridID_T ret = (nX + nZ*_w);
		if (ret >= _size)
			return INVALID_ID;
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
        ASSERT(false, "error");
        return false;
    };

    bool LeaveZone(const T &id, GridID_T gridid)
    {
        if (CheckGridID(gridid))
        {
            (_grids + gridid)->LeaveGrid(id);
            return true;
        }
        ASSERT(false, "error");
        return false;
    };

    void GetAllIDsByRadius(GridID_T gridID, s32 gridRadius, const ElementCallBackFun &fun)
    {
        if ( fun == nullptr )
        {
            ASSERT(false, "fun is null");
            return;
        }
        if (!CheckGridID(gridID))
        {
            ASSERT(false, "gridid error, gridid = %d", gridID);
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
                ASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
                if((_grids + temp)->TraversingCallAll( fun)) return;
            }
        }
    };
    void GetIDsByRadiusFromInner(GridID_T gridID, s32 gridRadius, const ElementCallBackFun &fun)
    {
        if ( fun == nullptr)
        {
            ASSERT(false, "fun is null");
            return;
        }
        if (!CheckGridID(gridID))
        {
            ASSERT(false, "gridid error, gridid = %d", gridID);
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
                                ASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
                                if ((_grids + temp)->TraversingCallAll(fun)) return;
                            }
                        }
                    }
                    else
                    {
                        if (CheckX(startX))
                        {
                            GridID_T temp = startX + tempZ * _w;
                            ASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
                            if ((_grids + temp)->TraversingCallAll(fun)) return;
                        }
                        if (CheckX(endX))
                        {
                            GridID_T temp = endX + tempZ * _w;
                            ASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
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
            ASSERT(false, "fun is null");
            return;
        }
        if (!(CheckGridID(gridA) && CheckGridID(gridB)))
        {
            ASSERT(false, "GetZoneByRadius_Sub idzoneA or idzoneB error, A = %d, B = %d", gridA, gridB);
            return;
        }
        Rect vectA;
        Rect vectB;
        GetRectInRadius(&vectA, gridA, radius);
        GetRectInRadius(&vectB, gridB, radius);
        for (int nz = vectA.startZ; nz <= vectA.endZ; nz++)
        {
            for (int nx = vectA.startX; nx <= vectA.endX; nx++)
            {
                if (!vectB.IsContain(nx, nz))
                {
                    GridID_T zoneid = nx + nz * _w;
                    ASSERT(zoneid < _size && zoneid >= 0, "temp error, zoneid = %d", zoneid);
                    if ((_grids + zoneid)->TraversingCallAll(fun))return;
                }
            }
        }
    };

    void GetAIntersectionBIds(GridID_T gridA, GridID_T gridB, s32 radius, const ElementCallBackFun &fun)
    {
        if (fun == nullptr)
        {
            ASSERT(false, "fun is null");
            return;
        }
        if (!(CheckGridID(gridA) && CheckGridID(gridB)))
        {
            ASSERT(false, "GetZoneByRadius_Sub idzoneA or idzoneB error, A = %d, B = %d", gridA, gridB);
            return;
        }
        Rect vectA;
        Rect vectB;
        GetRectInRadius(&vectA, gridA, radius);
        GetRectInRadius(&vectB, gridB, radius);
        for (int nz = vectA.startZ; nz <= vectA.endZ; nz++)
        {
            for (int nx = vectA.startX; nx <= vectA.endX; nx++)
            {
                if (vectB.IsContain(nx, nz))
                {
                    GridID_T zoneid = nx + nz * _w;
                    ASSERT(zoneid < _size && zoneid >= 0, "temp error, zoneid = %d", zoneid);
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
    void GetRectInRadius(Rect *rect, GridID_T gridId, s32 nRadius)
    {
        s32 x = gridId % _w;
        s32 z = gridId / _w;
        rect->startX = (x - nRadius > 0) ? (x - nRadius) : 0;
        rect->startZ = (z - nRadius > 0) ? (z - nRadius) : 0;
        rect->endX = (x + nRadius >= _w) ? _w - 1 : x + nRadius;
        rect->endZ = (z + nRadius >= _h) ? _h - 1 : z + nRadius;
    };
private:
    WORLD_POS2D _leftUpPos;
    s32 _w;
    s32 _h;
    s16 _size;
    s32 _gridLen;
    Grid *_grids;
};

#endif
