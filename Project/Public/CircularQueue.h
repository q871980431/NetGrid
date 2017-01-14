#ifndef __Circluar_Queue_h__
#define __Circluar_Queue_h__
#include "MultiSys.h"
#include <limits>

template< typename T >
class CircluarQueue
{
public:
	CircluarQueue(u32 size) :_size(size), _front(0), _tail(0), _queue(nullptr)
	{
		if (_size&_size-1)
			_size = RoundupPowerOf2(_size);
        _queue = new T[_size];
	}
	~CircluarQueue()
	{
		SAFE_DELETE_ARRAY(_queue);
	}

	inline T * Pop()
	{
		if (_front == _tail)return nullptr;
		return &_queue[_front++&(_size - 1)];
	}
	inline bool Push(const T &t)
	{
		if (_tail - _front >= _size)return false;
		_queue[_tail++ &(_size - 1)] = t;
		return true;
	}

	inline T * Push()
	{
		if (_tail - _front >= _size)return nullptr;
		return &_queue[_tail++ &(_size - 1)];
	}
	inline u32	Size(){ return _tail - _front; };
	inline u32  Capacity(){ return _size; };
protected:
private:
	u32 RoundupPowerOf2(u32 num)
	{
		u32  temp = ~((std::numeric_limits<s32>::max)());
		while (!(temp & num)) temp >>= 1;
		return temp << 1;
	}

private:
	T	*_queue;
	u32	_front;
	u32	_tail;
	u32	_size;
};
#endif