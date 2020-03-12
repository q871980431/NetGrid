#ifndef __T_IndexPool_h__
#define __T_IndexPool_h__
#include "MultiSys.h"
#include "THeap.h"
#include <vector>

namespace tlib
{
	const s32 INVALID_INDEX = -1;
	class IndexNode
	{
	public:
		inline s32 GetIndex() { return _index; };
		inline void SetIndex(s32 index) { _index = index; };
	private:
		s32 _index{ INVALID_INDEX };
	};

	template<typename T, s32 BatchNum=64>
	class IndexPool
	{
	public:
		IndexPool(){
			InitHeapCapacity(BatchNum);
		}

		~IndexPool()
		{
			for (auto &node : _nodes)
				free(node);
			DEL _indexHeap;
		}
	
		template<typename ...Args>
		T * Create(Args... args)
		{
			s32 index = 0;
			if (!_indexHeap->Delete(index))
			{
				ExpandCapacity(BatchNum);
				if (!_indexHeap->Delete(index))
					return nullptr;
			}
			ASSERT(_nodes[index]->GetIndex() == INVALID_INDEX, "error");
			T *ret = new(_nodes[index]) T(args...);
			ret->SetIndex(index);
			return ret;
		}

		T * Find(s32 index)
		{
			if (index < 0 || index >(s32)_nodes.size())
				return nullptr;
			return (T*)_nodes[index];
		}

		void Recover(T *t)
		{
			s32 index = t->GetIndex();
			t->~T();
			t->SetIndex(INVALID_INDEX);
			_indexHeap->Insert(index);
		}

	private:
		void ExpandCapacity(s32 expandNum)
		{
			s32 oldSize = _nodes.size();
			for (s32 i = 0; i < expandNum; i++)
			{
				T *temp = (T*)malloc(sizeof(T));
				if (temp != nullptr)
				{
					((IndexNode*)temp)->SetIndex(INVALID_INDEX);
					_nodes.push_back(temp);
					_indexHeap->Insert(oldSize);
					oldSize++;
				}
			}
		}

		void InitHeapCapacity(s32 capacity)
		{
			_indexHeap = NEW THeap<s32>(capacity);
			for (s32 i = 0; i < capacity; i++)
			{
				T *temp = (T*)malloc(sizeof(T));
				if (temp != nullptr)
				{
					((IndexNode*)temp)->SetIndex(INVALID_INDEX);
					_nodes.push_back(temp);
					_indexHeap->Insert(i);
				}
			}
		}

	private:
		std::vector<T*>	_nodes;
		THeap<s32>	*_indexHeap;
	};
}
#endif