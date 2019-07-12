#ifndef __Circular_Buffer_h__
#define __Circular_Buffer_h__
#include "MultiSys.h"
#include <algorithm>
#include <limits>
#include <atomic>

class CircluarBuffer
{
public:
	CircluarBuffer(s32 size) :_buff(nullptr), _size(size), _wcount(0), _rcount(0)
	{
		if (_size&(_size - 1))_size = RoundupPowerOf2(_size);
        _buff = NEW char[_size];
		_mask = _size - 1;
	}
	~CircluarBuffer()
	{
		if ( _buff )
		{
			delete[] _buff;
			_buff = nullptr;
		}
	}

	s32 Write(const void *buffer, s32 len)
	{
		s32 l;
		l = GetUnUsed();
		if (len > l)
			len = l;
		s32 off = _wcount&_mask;
		l = min(len, _size - off);

		memcpy(_buff + off, buffer,  l);
		memcpy(_buff, (const char *)buffer + l, len - l);
        std::atomic_thread_fence(std::memory_order_release);

		_wcount += len;
		return len;
	}

	char * GetCanWriteBuff(s32 &len) { 
		if (0 == GetUnUsed())
			return nullptr;
		s32 realW = _wcount & _mask;
		s32 realR = _rcount & _mask;
		if (realW >= realR)
			len = _size - realW;
		else
			len = realR -realW;

		return _buff+realW; 
	}

	void WriteBuff(s32 size)
	{
		_wcount += size;
	}

	char * GetCanReadBuff(s32 &len)
	{
		if (_rcount == _wcount)
			return nullptr;
		s32 realW = _wcount & _mask;
		s32 realR = _rcount & _mask;
		if (realW > realR)
			len = _wcount - _rcount;
		else
			len = _size - realR;

		return _buff + realR;
	}

	char * TryReadBuff(void *buff, s32 len)
	{
		if (DataSize() < len)
			return nullptr;
		s32 realW = _wcount & _mask;
		s32 realR = _rcount & _mask;
		if (realW > realR)
			return _buff + realR;
		else
		{
			s32 l;
			l = min(len, _size - realR);
			memcpy(buff, _buff + realR, l);
			memcpy((char*)buff + l, _buff, len - l);
			return (char*)buff;
		}
	}

	s32 Read(void *buff, s32 len)
	{
		s32 l;		
		l = _wcount - _rcount;
		if (l == 0)
			return 0;
		if (len > l)
			len = l;
		s32 off = _rcount & _mask;
		l = min(len, _size - off);

		memcpy(buff, _buff + off, l);
		memcpy((char*)buff + l, _buff, len - l);
        std::atomic_thread_fence(std::memory_order_release);

		_rcount += len;
		return len;
	}

	void Read(s32 size)
	{
		_rcount += size;
	}

	inline s32 GetBufferSize(){ return _size; };
	inline s32 GetUnUsed() { return _size - (_wcount - _rcount); };
	inline s32 DataSize() { return _wcount - _rcount; }
private:
	s32 RoundupPowerOf2(s32 num)
	{
		s32  temp = ~(std::numeric_limits<s16>::max)();
		while (!(temp & num) ) temp >>= 1;
		return temp << 1;
	}
private:
	char	*_buff;
	s32		_size;
	s32		_wcount;
	s32		_rcount;
	s32		_mask;
};
#endif