#ifndef __Vector_h__
#define __Vector_h__
#include "MultiSys.h"

namespace VectorDefine
{
    enum 
    {
        X = 0,
        Z = 1,
        Y = 2,
    };
}
template<u8 N = 2>
struct Vector 
{
    s32 x[N];
    Vector()
    {
        memset(&x, 0, sizeof(x));
    }
    Vector(s32 n)
    {
        for (u8 i = 0; i < N; i++)
            x[i] = n;
    }

    Vector(const Vector &right)
    {
        for (u8 i = 0; i < N; i++)
            x[i] = right.x[i];
    }
    inline u8 Dimension(){ return N; };
    inline s32 & operator[](s8 index){ ASSERT(index < N, "index error"); return x[index]; };
    inline s32  GetVal(s8 index) const{ ASSERT(index < N, "index error"); return x[index]; };
    Vector & operator=(const Vector &val){
        for (u8 i = 0; i < N; i++)
            x[i] = val.x[i];
        return *this;
    }
    Vector & operator-(const Vector &val){
        for (u8 i = 0; i < N; i++)
            x[i] -= val.x[i];
        return *this;
    }

    Vector & operator+(const Vector &val){
        for (u8 i = 0; i < N; i++)
            x[i] += val.x[i];
        return *this;
    }


    s64 Distance(){
        s64 rt = 0;
        for (u8 i = 0; i < N; i++)
           rt += x[i] * x[i];
        return rt;
    }

    s64 Distance(Vector &val)
    {
        s64 rt = 0;
        for (u8 i = 0; i < N; i++)
        {
            s32 tmp = val.x[i] - x[i];
            rt += tmp * tmp;
        }

        return rt;
    }
};
template <u8 N = 2>
struct MBRect
{
    Vector<N> left;
    Vector<N> right;

    MBRect()
    {
        memset(&left, 0, sizeof(left));
        memset(&right, 0, sizeof(right));
    }
    MBRect(const MBRect &val)
    {
        memcpy(left, val.left, sizeof(left));
        memcpy(right, val.right, sizeof(right));
    }
    bool IsContain(const MBRect &val) const
    {
        for (u8 i = 0; i < N; i++)
        {
            if (val.left[i] < left[i] || val.right[i] > right[i])
                return false;
        }

        return true;
    }

    bool Valied()
    {
        for (u8 i = 0; i < N; i++)
        {
            if (left[i] == right[i])
                return false;
        }

        return true;
    }
};

typedef Vector<3> Vector3D;
typedef MBRect<3> MBRect3D;

#endif