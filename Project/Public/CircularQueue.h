#ifndef __Circluar_Queue_h__
#define __Circluar_Queue_h__
#include "MultiSys.h"
#include <limits>
#include <atomic>

template< typename T >
class CircluarQueue
{
public:
	CircluarQueue(u32 size) :_queue(nullptr), _front(0), _tail(0), _size(size)
	{
		if (_size&(_size-1))
			_size = RoundupPowerOf2(_size);
        _queue = new T[_size];
		_mask = _size - 1;
	}
	~CircluarQueue()
	{
		SAFE_DELETE_ARRAY(_queue);
	}

	inline bool Pop(T &val)
	{
		if (_front == _tail)return false;
		val = _queue[_front&_mask];
		std::atomic_thread_fence(std::memory_order_release);
		_front++;
		return true;
	}
	inline bool Push(const T &t)
	{
		if (_tail - _front >= _size)return false;
		_queue[_tail&_mask] = t;
		std::atomic_thread_fence(std::memory_order_release);
		_tail++;
		return true;
	}

	inline void TryPush(const T &t)
	{
		while (_tail - _front >= _size)
			MSLEEP(1);
		_queue[_tail++ &_mask] = t;
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
	u32 _mask;
};
#endif