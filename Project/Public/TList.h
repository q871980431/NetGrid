#ifndef __T_List_h__
#define __T_List_h__
#include "MultiSys.h"

template< class T >
struct TListNode
{
	T _data;
	TListNode	*_prev;
	TListNode	*_next;
    TListNode() :_prev(nullptr), _next(nullptr){};
};

template< class T >
void InsertHead(T *&head, T *newNode)
{
    ASSERT(newNode, "new node is null");
    newNode->_next = head;
    if (head)
        head->_prev = newNode;
    head = newNode;
}

template< class T >
inline void InsertTail(T *tail, T *newNode)
{
    ASSERT(tail, "head is null");
    ASSERT(newNode, "new node is null");
    tail->_next = newNode;
    newNode->_prev = tail;
}

template< class T >
void RemoveNode( T *node)
{
    ASSERT(node, "node is null ptr");
    if (node->_next != nullptr)
        node->_next->_prev = node->_prev;
    if (node->_prev != nullptr)
        node->_prev->_next = node->_next;
    node->_prev = nullptr;
    node->_next = nullptr;
}


template< class T >
class List
{
public:
	List()
	{
		_size = 0;

		_head._next = &_tail;
		_head._prev = &_head;	
		_tail._next = &_tail;
		_tail._prev = &_head;
	}

	void Insert( T *pNode )
	{
		pNode->_next = &_tail;
		pNode->_prev = _tail._prev;
		_tail._prev->_next = pNode;
		_tail._prev = pNode;
		_size++;
	}

	bool Delete( T *pNode )
	{
		T *pPoints = _head->_next;
		while( pPoints != &_tail )
		{
			if ( pPoints == pNode)
			{
				pPoints->m_pRrev->_next = pPoints->_next;
				pPoints->_next->m_pRrev = pPoints->m_pRrev;
				pPoints->m_pRrev = NULL;
				pPoints->_next = NULL;
				_size--;
				return true;
			}else
			{
				pPoints = pPoints->_next;
			}
		}
		return false;
	}
	
	void SafeDelete()
	{
		T *pPoint = _head._next;
		_head._next = NULL;
		while( pPoint != &_tail )
		{
			SAFE_DELETE( pPoint->m_pNode );
			pPoint = pPoint->_next;
			SAFE_DELETE( pPoint->_prev );
		}
	}
private:
    TListNode<T>    *_head;
    TListNode<T>	*_tail;
	s32			    _size;
};
#endif