#include "Zone.h"


template< typename T >
Zone::Zone( const WORLD_POS2D &leftUpPos, const WORLD_POS2D &rightDownPos, s32 gridLen)
{
    TASSERT(leftUpPos.x < rightDownPos.x, "x error");
    TASSERT(leftUpPos.z < rightDownPos.z, "z error");
    _leftUpPos = leftUpPos;
    _gridLen = gridLen;
    s32 lenX = rightDownPos.x - leftUpPos.x;
    s32 lenZ = rightDownPos.z - leftUpPos.z;
    _W = lenX / gridLen;
    _H = lenZ / gridLen;
    if (lenX % gridLen > 0)
        ++_W;
    if (lenZ % gridLen > 0)
        ++_H;
    _size = _W * _H;
    _grids = new Grid<T>[_size];
    TASSERT(_grids, "grids is null");
}
template< typename T >
GridID_T Zone::CalcZoneID( WORLD_POS2D pos)
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

    GridID_T ret = (nX + nZ*_W);
    TASSERT(ret < _size, "grid id error");
    return ret;
}

template< typename T >
bool Zone::EnterZone(const T &id, GridID_T gridid)
{
    TASSERT(gridid < _size, "grid id error");
    if (gridid >= _size)
        return false;
    (_grids + gridid)->EnterGrid(id);
    return true;
}
template< typename T >
bool Zone::LeaveZone(const T &id, GridID_T gridid)
{
    TASSERT(gridid < _size, "grid id error");
    if (gridid >= _size)
        return false;
    (_grids + gridid)->LeaveGrid(id);
    return true;
}

template< typename T >
void Zone::GetAllIDsByRadius(GridID_T gridID, s32 gridRadius, s32 *ids, s32 count, s32 &num)
{
    num = 0;
    if (INVALID_ID == gridID || gridID >= _size)
    {
        TASSERT(false, "gridid error, gridid = %d", gridID);
        return;
    }
    s32 nX = gridID % _W;
    s32 nZ = gridID / _W;
    s32 nStartX = 0;
    s32 nStartZ = 0;
    s32 nEndX = 0;
    s32 nEndZ = 0;
    nStartX = (nX - gridRadius > 0) ? (nX - gridRadius) : 0;
    nStartZ = (nZ - gridRadius > 0) ? (nZ - gridRadius) : 0;
    nEndX = (nX + gridRadius >= _W) ? _W - 1 : nX + gridRadius;
    nEndZ = (nZ + gridRadius >= _H) ? _H - 1 : nZ + gridRadius;

    for (nZ = nStartZ; nZ <= nEndZ; nZ++)
    {
        for (nX = nStartX; nX <= nEndX; nX++)
        {
            GridID_T temp = nX + nZ * _W;
            TASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
            (_grids + temp)->TraversingCallAll(ids, num, count);
            if (num >= count)return;
        }
    }
}
template< typename T >
void Zone::GetIDsByRadiusFromInner(GridID_T gridID, s32 gridRadius, s32 *ids, s32 count, s32 &num)
{
    num = 0;
    if (INVALID_ID == gridID || gridID >= _size)
    {
        TASSERT(false, "gridid error, gridid = %d", gridID);
        return;
    }
    int nX = gridID % _W;
    int nZ = gridID / _W;
    int nStartX = 0;
    int nStartZ = 0;
    int nEndX = 0;
    int nEndZ = 0;
    nStartX = (nX - gridRadius > 0) ? (nX - gridRadius) : 0;
    nStartZ = (nZ - gridRadius > 0) ? (nZ - gridRadius) : 0;
    nEndX = (nX + gridRadius >= _W) ? _W - 1 : nX + gridRadius;
    nEndZ = (nZ + gridRadius >= _H) ? _H - 1 : nZ + gridRadius;

    for (s32 i = 0; i <= gridRadius; i++)
    {
        if (i == 0)
        {
            GridID_T temp = nX + nZ * _W;
            TASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
            (_grids + temp)->TraversingCallAll(ids, num, count);
            if (num >= count)return;
        }
        else
        {
            s32 tx = nX - i;
            s32 ex = nX + i;
            s32 tz = nZ - i;
            s32 ez = nZ + i;
            if (tx < nStartX) tx = nStartX;
            if (ex > nEndX)ex = nEndX;
            if (tz < nStartZ) tz = nStartZ;
            if (ez > nEndZ)ez = nEndZ;
            for (s32 j = 0; tz <= ez; tz++, j++)
            {

                if (j == 0 || tz == ez)
                {
                    if (tz > nStartZ && tz < nEndZ)
                    {
                        for (s32 tmpx = tx; tmpx <= ex; tmpx++)
                        {
                            GridID_T temp = tmpx + tz * _W;
                            TASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
                            (_grids + temp)->TraversingCallAll(ids, num, count);
                            if (num >= count)return;
                        }
                        continue;
                    }
                }
                s32 tmpx = tx;
                if (tmpx != nStartX)
                {
                    GridID_T temp = tmpx + tz * _W;
                    TASSERT(temp < _size  && temp >= 0, "temp error, temp = %d", temp);
                    (_grids + temp)->TraversingCallAll(ids, num, count);
                    if (num >= count)return;
                }
                tmpx = ex;
                if (tmpx != nEndX)
                {
                    GridID_T temp = tmpx + tz * _W;
                    TASSERT(temp < _size && temp >= 0, "temp error, temp = %d", temp);
                    (_grids + temp)->TraversingCallAll(ids, num, count);
                    if (num >= count)return;
                }
            }
        }
    }
}
template< typename T >
void Zone::GetCutIds(GridID_T gridA, GridID_T gridB, s32 *ids, s32 count, s32 &num, s32 radius)
{
    num = 0;
    if (!(CheckGridID(gridA) && CheckGridID(gridB)))
    {
        TASSERT(false ,"GetZoneByRadius_Sub idzoneA or idzoneB error, A = %d, B = %d", gridA, gridB);
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
                GridID_T zoneid = nx + nz * _W;
                TASSERT(zoneid < _size && zoneid >= 0, "temp error, temp = %d", zoneid);
                (_grids + zoneid)->TraversingCallAll(ids, num, count);
                if (num >= count)return;
            }
        }
    }
}
template< typename T >
void Zone::GetRectInRadius(VRECT *pRect, GridID_T gridId, s32 nRadius)
{
    if (INVALID_ID == gridId || gridId >= _size)
    {
        TASSERT(false, "gridid error, gridid = %d", gridId);
        return;
    }
    int nX = gridId % _W;
    int nZ = gridId / _W;
    pRect->startX = (nX - nRadius > 0) ? (nX - nRadius) : 0;
    pRect->startZ = (nZ - nRadius > 0) ? (nZ - nRadius) : 0;
    pRect->endX = (nX + nRadius >= _W) ? _W - 1 : nX + nRadius;
    pRect->endZ = (nZ + nRadius >= _H) ? _H - 1 : nZ + nRadius;
}
template< typename T >
void Grid::TraversingCallAll(s32 *ids, s32 &num, s32 count)
{
    for (auto iter = _data.begin(); iter != _data.end(); iter++)
    {
        if (num >= count)return;
        ids[num++] = *iter;
    }
}