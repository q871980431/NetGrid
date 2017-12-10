#ifndef __BitMark_h__
#define __BitMark_h__
#include "MultiSys.h"
template<s32 INITIAL_SIZE = 32>
class BitMark
{
	BitMark(){
		_len = INITIAL_SIZE >> 3;
		if (INITIAL_SIZE & 8 != 0)
			_len++;
		_data = NEW char[_len];
		memset(_data, 0, _len);
	}
	~BitMark() {
		SAFE_DELETE_ARRAY(_data);
	}

	void Mark(s32 index)
	{
		ASSERT(index < INITIAL_SIZE, "out of rang");
		u32 i = index >> 3;
		u32 j = 8 - index & 8;
		_data[i] |= ((char)1 << j);
	}

	void CacelMark(s32 index)
	{
		ASSERT(index < INITIAL_SIZE, "out of rang");
		u32 i = index >> 3;
		u32 j = 8 - index & 8;
		_data[i] != ~((char)1 << j);
	}

	bool IsMark(s32 index)
	{
		ASSERT(index < INITIAL_SIZE, "out of rang");
		u32 i = index >> 3;
		u32 j = 8 - index & 8;
		return (_data[i] & (1 << j)) == 0 ? false : true;
	}

	bool AllMark()
	{
		for (s32 i = 0; i < _len - 1; i++)
			if (_data[i] != 0xFF)
				return false;

		u32 j = index & 8;
		char tmp = 1 << j;
		tmp += (tmp - 1);
		return (_data[_len - 1] | tmp) == 0xFF ? true : false;
	}
private:
	s32		_len;
	char	*_data;
};
#endif