#ifndef __T_List_h__
#define __T_List_h__
#include "MultiSys.h"
#include <mutex>
namespace tlib
{
    namespace linear{
        class LinkList;
        class ILinkNode
        {
        public:
            ILinkNode():_prev(nullptr), _next(nullptr), _host(nullptr){};
            ILinkNode *_prev;
            ILinkNode *_next;
            LinkList  *_host;
        };
        class LinkList
        {
        public:
            LinkList() :_head(nullptr), _tail(nullptr), _size(0) {};
        public:
            ILinkNode * HeadRemove()
            {
                if (_size == 0) return nullptr;

                ILinkNode *tmp = _head;
                _head = _head->_next;
                if (_head == nullptr)
                    _tail = nullptr;
                else
                    _head->_prev = nullptr;
                CleanNodeIndex(tmp);
                --_size;
                return tmp;
            }

            ILinkNode * TailRemove()
            {
                if (_size == 0) return nullptr;
                ILinkNode *tmp = _tail;
                _tail = _tail->_prev;
                if (_tail == nullptr)
                    _head = nullptr;
                else
                    _tail->_next = nullptr;
                --_size;

                CleanNodeIndex(tmp);
                return tmp;
            }

            bool Remove(ILinkNode *node)
            {
                ASSERT(node && node->_host == this, "error");
                if (node && node->_host)
                {
                    if (node->_prev)
                        node->_prev->_next = node->_next;
					else
					{
						_head = node->_next;
						if (_head)
							_head->_prev = nullptr;
					}

                    if (node->_next != nullptr)
                        node->_next->_prev = node->_prev;
					else
					{
						_tail = node->_prev;
						if (_tail)
							_tail->_next = nullptr;
					}

                    CleanNodeIndex(node);
                    --_size;

                    return true;
                }
                return false;
            }

            void HeadInsert(ILinkNode *node)
            {
                ASSERT(node && node->_host == nullptr, "error");
                if (node && node->_host == nullptr)
                {
                    node->_host = this;

					if (_head == nullptr)
					{
						_head = _tail = node;
						node->_prev = node->_next = nullptr;
					}
					else
					{
						node->_prev = nullptr;
						node->_next = _head;
						_head->_prev = node;
						_head = node;
					}
                    ++_size;
                }
            }

            void TailInsert(ILinkNode *node)
            {
                ASSERT(node && node->_host == nullptr, "error");
                if (node && node->_host == nullptr)
                {
                    node->_host = this;

					if (_tail == nullptr)
					{
						_head  = _tail = node;
						node->_prev = node->_next = nullptr;
					}
					else
					{
						node->_prev = _tail;
						node->_next = nullptr;
						_tail->_next = node;
						_tail = node;
					}
                    ++_size;
                }
            }

			void TailMerge(LinkList *list)
			{
				ASSERT(list->_size != 0, "error");
				ILinkNode **tailInsertPos = GetTailInsertPos();
				list->_head->_prev = _tail;
				*tailInsertPos = list->_head;

				ILinkNode *temp = _tail->_next;
				while (temp)
				{
					temp->_host = this;
					temp = temp->_next;
				}
				_tail = list->_tail;
				_size += list->_size;
				list->_head = nullptr;
				list->_tail = nullptr;
				list->_size = 0;
			}

			inline ILinkNode ** GetTailInsertPos(){return _tail != nullptr ? &_tail->_next : &_head;}
			inline ILinkNode ** GetHeadInsertPos(){	return _head != nullptr ? &_head->_prev : &_tail;}
            inline ILinkNode * Head(){ return _head; };
            inline ILinkNode * Tail(){ return _tail; };
			inline void Swap(LinkList *lists)
			{
				if (this == lists)
					return;

				std::swap(_size, lists->_size);
				ILinkNode *tmp = _head;
				while (tmp != nullptr)
				{
					tmp->_host = lists;
					tmp = tmp->_next;
				}

				std::swap(_head, lists->_head);
				std::swap(_tail, lists->_tail);
				tmp = _head;
				while (tmp != nullptr)
				{
					tmp->_host = this;
					tmp = tmp->_next;
				}
			}
        protected:
            inline void CleanNodeIndex(ILinkNode *node)
            {
                node->_host = nullptr;
                node->_prev = nullptr;
                node->_next = nullptr;
            }
        private:
            ILinkNode *_head;
            ILinkNode *_tail;
            s32        _size;
		};

		template<typename T>
		struct SimpleList
		{
			T *head;
			T *tail;
			SimpleList() :head(nullptr), tail(nullptr) {};
		};

		template<typename L, typename T>
		void PushHead(L &list, T *node)
		{
			if (list.head == nullptr) {
				ASSERT(list.tail == nullptr, "wtf");
				list.head = node;
				list.tail = node;
			}
			else {
				ASSERT(list.tail != nullptr, "wtf");
				node->_next = list.head;
				list.head = node;
			}
		};

		template<typename L, typename T>
		void PushTail(L &list, T *node)
		{
			if (list.head == nullptr) {
				ASSERT(list.tail == nullptr, "wtf");
				list.head = node;
				list.tail = node;
			}
			else {
				ASSERT(list.tail != nullptr, "wtf");
				list.tail->next = node;
				list.tail = node;
			}
		}

		template<typename L>
		void MergeList(L& to, L& from)
		{
			if (from.head == nullptr)
				return;
			else {
				ASSERT(from.tail != nullptr, "wtf");
				if (to.head == nullptr) {
					to.head = from.head;
					to.tail = from.tail;
				}
				else {
					to.tail->next = from.head;
					to.tail = from.tail;
				}
				from.head = from.tail = nullptr;
			}
		}

		template<typename L, typename T>
		void MergeListByLock(L& to, L& from, T &lock)
		{
			if (from.head == nullptr)
				return;
			else {
				ASSERT(from.tail != nullptr, "wtf");
				std::lock_guard<T> guard(lock);
				if (to.head == nullptr) {
					to.head = from.head;
					to.tail = from.tail;
				}
				else {
					to.tail->next = from.head;
					to.tail = from.tail;
				}
				from.head = from.tail = nullptr;
			}
		}

		template<typename L, typename T>
		T * PopHead(L &list)
		{
			if (list.head == nullptr)
				return nullptr;

			T * first = list.head;
			list.head = first->next;
			if (list.head == nullptr)
				list.tail = nullptr;

			return first;
		}

    }
}
#endif