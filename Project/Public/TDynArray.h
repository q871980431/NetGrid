#ifndef __T_DynArray_h__
#define __T_DynArray_h__
#include "MultiSys.h"
#include <limits.h>

namespace tlib{

    template <class T, int INITIAL_SIZE = 16>
    class TDynArray
    {
    public:
        TDynArray() {
            _mem = _pool;
            _allocated = INITIAL_SIZE;
            _size = 0;
        }

        ~TDynArray() {
            if (_mem != _pool) {
                delete[] _mem;
            }
        }

        void Clear() {
            _size = 0;
        }

        void Push(T t) {
            ASSERT(_size < INT_MAX, "_size too big");
            EnsureCapacity(_size + 1);
            _mem[_size++] = t;
        }

        T* PushArr(s32 count) {
            ASSERT(count >= 0, "count exception");
            ASSERT(_size <= INT_MAX - count, "size too big");
            EnsureCapacity(_size + count);
            T* ret = &_mem[_size];
            _size += count;
            return ret;
        }

        T Pop() {
            ASSERT(_size > 0, "size less than 0");
            return _mem[--_size];
        }

        void PopArr(s32 count) {
            ASSERT(count >= 0, "count exception");
			ASSERT(_size >= count, "count exception");
            _size -= count;
        }

        bool Empty() const					{
            return _size == 0;
        }

        T& operator[](s32 index){
            ASSERT(index >= 0 && index < _size, "index invalied");
            return _mem[index];
        }

        const T& operator[](s32 index) const	{
            ASSERT(index >= 0 && index < _size, "index invalied");
            return _mem[index];
        }

        s32 Size() const {
            ASSERT(_size >= 0, "size invalied");
            return _size;
        }

        s32 Capacity() const {
            ASSERT(_allocated >= INITIAL_SIZE, "allocated error");
            return _allocated;
        }

        const T* Mem() const {
            ASSERT(_mem, "mem point exception");
            return _mem;
        }

        T* Mem(){
            ASSERT(_mem, "mem point exception");
            return _mem;
        }

    private:
        TDynArray(const TDynArray&); // not supported
        void operator=(const TDynArray&); // not supported

        void EnsureCapacity(s32 cap) {
            ASSERT(cap > 0, "cap exception");
            if (cap > _allocated) {
                ASSERT(cap <= INT_MAX / 2, "cap exception");
                s32 newAllocated = cap * 2;
                T* newMem = new T[newAllocated];
                memcpy(newMem, _mem, sizeof(T)*_size);	// warning: not using constructors, only works for PODs
                if (_mem != _pool) {
                    delete[] _mem;
                }
                _mem = newMem;
                _allocated = newAllocated;
            }
        }

        T*  _mem;
        T   _pool[INITIAL_SIZE];
        s32 _allocated;		// objects allocated
        s32 _size;			// number objects in use
    };
}

#endif