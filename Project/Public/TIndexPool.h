#ifndef __T_IndexPool_h__
#define __T_IndexPool_h__
#include "MultiSys.h"
namespace tlib
{
	class IndexNode
	{
	public:
		inline s32 GetIndex() { return _index; };
		inline void SetIndex(s32 index) { _index = index; };
	private:
		s32 _index{-1};
	};

	template<typename T, s32 InitSize=1024>
	class IndexPool
	{
	public:
		IndexPool(){
		
		}

		template<typename ...Args>
		T * Create(Args ...)
		{

		}

		void Recover(T *)
		{

		}

	private:
	protected:
	private:
		std::vector<IndexNode*>	_index;
		s32
	};
}
#endif