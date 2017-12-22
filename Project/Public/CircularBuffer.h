#ifndef __Circular_Buffer_h__
#define __Circular_Buffer_h__
#include "MultiSys.h"
#include <algorithm>
#include <limits>
#include <atomic>

class CircluarBuffer
{
public:
	CircluarBuffer(u16 size) :_size(size), _buff(nullptr), _wcount(0), _rcount(0)
	{
		if (_size&(_size - 1))_size = RoundupPowerOf2(_size);
        _buff = NEW char[_size];
	}
	~CircluarBuffer()
	{
		if ( _buff )
		{
			delete[] _buff;
			_buff = nullptr;
		}
	}
	u16 Write(const void *buffer, u16 len)
	{
		u16 l;
		len = min(len, (u16)(_size - _wcount + _rcount));
		l = min(len, (u16)(_size - _wcount&(_size - 1)));
		memcpy(_buff + _wcount, buffer,  l);
		memcpy(_buff, buffer, len - l);
        std::atomic_signal_fence(std::memory_order_release);

		_wcount += len;
		return len;
	}
	u16 Read(void *buff, u16 len)
	{
		u16 l;
		len = min(len, (u16)(_wcount - _rcount));
		l = min(len, (u16)(_size - _rcount &(_size - 1)));
		memcpy(buff, _buff + (_rcount&(_size - 1)), l);
		memcpy((char*)buff + l, _buff, len - l);
        std::atomic_signal_fence(std::memory_order_release);

		_rcount += len;
		return len;
	}

	inline u16 GetBufferSize(){ return _size; };
private:
	u16 RoundupPowerOf2(u16 num)
	{
		u16  temp = ~(std::numeric_limits<s16>::max)();
		while (!(temp & num) ) temp >>= 1;
		return temp << 1;
	}
private:
	char	*_buff;
	u16		_size;
	u16		_wcount;
	u16		_rcount;
};
#endif