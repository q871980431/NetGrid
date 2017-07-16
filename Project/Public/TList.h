﻿#ifndef __T_List_h__
#define __T_List_h__
#include "MultiSys.h"
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
                    if (node->_prev != nullptr)
                        node->_prev->_next = node->_next;
                    if (node->_next != nullptr)
                        node->_next->_prev = node->_prev;
                    if (_head == node)
                        _head = node->_next;
                    if (_tail == node)
                        _tail = node->_prev;
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
                    node->_next = _head;
                    node->_host = this;

                    if (_head == nullptr)
                        _tail = node;
                    else
                        _head->_prev = node;
                    _head = node;
                    ++_size;
                }
            }

            void TailInsert(ILinkNode *node)
            {
                ASSERT(node && node->_host == nullptr, "error");
                if (node && node->_host == nullptr)
                {
                    node->_prev = _tail;
                    node->_host = this;

                    if (_tail == nullptr)
                        _head = node;
                    else
                        _tail->_next = node;
                    _tail = node;
                    ++_size;
                }
            }

            inline ILinkNode * Head(){ return _head; };
            inline ILinkNode * Tail(){ return _tail; };
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

    }
}
#endif