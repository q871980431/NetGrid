#ifndef __THeap_h__
#define __THeap_h__
#include "MultiSys.h"
template<typename T>
class THeap
{
public:
	THeap(s32 size) {
		_size = size;
		_items = NEW T[_size]();
		_heapSize = 0;
		_new = true;
	};

	THeap(T *items, s32 size, s32 heapSize) :_items(items), _size(size), _heapSize(heapSize), _new(false)
	{
		HeapFromat();
	}

	THeap() :_items(nullptr), _size(0), _heapSize(0), _new(false)
	{

	}

	~THeap() 
	{
		Reset();
	}

	void Init(T *items, s32 size, s32 heapSize)
	{
		if (_items == nullptr)
		{
			_items = items;
			_size = size;
			_heapSize = heapSize;
			_new = false;
			HeapFromat();
		}
	}

	void Reset()
	{
		if (_new)
		{
			DEL[] _items;
		}
		_items = nullptr;
	}

	bool Insert(T &item)
	{
		if (_heapSize == _size)
		{
			if (!_new)
				return false;
			else
				ExpanCapacity();
		}
		_items[_heapSize] = item;
		FileterUp(_heapSize);
		_heapSize++;
		return true;
	}

	bool Delete(T &item)
	{
		if (_heapSize == 0)
			return false;
		item = _items[0];
		_items[0] = _items[_heapSize - 1];
		FileterDown(0);
		_heapSize--;
		return true;
	}

	bool Remove(const T &item)
	{
		if (_heapSize == 0)
			return false;
		s32 i = _heapSize - 1;
		for (; i >= 0; i--)
		{
			if (_items[i] == item)
				break;
		}

		if (i < 0)
			return false;

		if (i != _heapSize -1)
		{
			_items[i] = _items[_heapSize - 1];
			FileterDown(i);
		}
		_heapSize--;
		return true;
	}

	const T * GetHeap(s32 &size) { size = _heapSize; return _items; };

	void HeapSort() {
		T del;
		s32 size = _heapSize;
		while (Delete(del))
		{
			_items[_heapSize] = del;
		}
		_heapSize = size;
	};
	inline s32 GetCapacity() { return _size; };
	inline s32 Size() { return _heapSize; };

private:
	void FileterUp(s32 i) 
	{
		s32 currentPos, parentPos;
		T target;
		currentPos = i;
		parentPos = (i - 1) / 2;
		target = _items[i];
		while (currentPos != 0)
		{
			if (_items[parentPos] <= target)
				break;
			else
			{
				_items[currentPos] = _items[parentPos];
				currentPos = parentPos;
				parentPos = (currentPos - 1) / 2;
			}
		}
		if (i != currentPos)
			_items[currentPos] = target;
	}

	void FileterDown(s32 i)
	{
		s32 currentPos, childPos;
		currentPos = i;
		childPos = currentPos * 2 + 1;
		T target = _items[i];
		while (childPos < _heapSize)
		{
			if ((childPos + 1 < _heapSize) && (_items[childPos + 1] < _items[childPos]))
				childPos = childPos + 1;
			if (target <= _items[childPos])
				break;
			else
			{
				_items[currentPos] = _items[childPos];
				currentPos = childPos;
				childPos = currentPos * 2 + 1;
			}
		}
		if (i != currentPos)
			_items[currentPos] = target;
	}

	void HeapFromat()
	{
		s32 parentPos = (_heapSize - 1) / 2;
		while (parentPos >= 0)
		{
			FileterDown(parentPos);
			parentPos--;
		}
	}

	void ExpanCapacity()
	{
		s32 nowSize = _size * 2;
		T *tmp = NEW T[nowSize]();
		memcpy(tmp, _items, sizeof(T) * _size);
		_size = nowSize;
		DEL[] _items;
		_items = tmp;
	}
private:
	s32 _size;
	T *	_items;
	s32 _heapSize;
	bool _new;
};

#endif
