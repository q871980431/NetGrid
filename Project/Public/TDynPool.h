#ifndef __T_Pool_h__
#define __T_Pool_h__
#include "MultiSys.h"
#include "TDynArray.h"
namespace tlib
{

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
    void RemoveNode(T *node)
    {
        ASSERT(node, "node is null ptr");
        if (node->_next != nullptr)
            node->_next->_prev = node->_prev;
        if (node->_prev != nullptr)
            node->_prev->_next = node->_next;
        node->_prev = nullptr;
        node->_next = nullptr;
    }

    template< typename T, s32 block_count = 1>
    class TDynPool
    {
        enum
        {
            IN_USE = 0,
            IS_FREE,
#ifdef WIN32
	#if(_MSC_VER <= 1800)
				CHUNK_SIZE = (16 * 1024) / (128),
	#else
				CHUNK_SIZE = sizeof(T),
	#endif
#else
            CHUNK_SIZE = (16 * 1024) / (128),
#endif // WIN32
        };
        struct Chunk;
        struct Block
        {
            s32                count;
            TListNode<Chunk>   chunks[CHUNK_SIZE];
        };
        struct Chunk
        {
            s8 buff[sizeof(T)];
            TListNode<Block> *parent;
            s8 type;
            s32 mark;
#ifdef  _DEBUG
            char file[MAX_PATH];
            s32 line;
#endif
        };
        typedef TListNode<Chunk> ChunkNode;
        typedef TListNode<Block> BlockNode;
    public:
		TDynPool() :_currentAllocs(0), _maxAllocs(0), _blockCount(0), _chunkRoot(nullptr), _blokRoot(nullptr)
        {
            AllocBlock(block_count);
        }

        ~TDynPool()
        {
            while (_blokRoot)
            {
                BlockNode *temp = _blokRoot;
                _blokRoot = _blokRoot->_next;
                SAFE_DELETE(temp);
            }
        }

#ifdef _DEBUG
        T * Create(const char *file, s32 line)
        {

            ChunkNode *chunk = GetChunk(file, line);
#else
        T * Create()
        {

            ChunkNode *chunk = GetChunk();
#endif
            ASSERT(chunk, "chunk is null");
            return new(chunk->_data.buff)T();
        }

        template<typename... Args >
#ifdef _DEBUG
        T *Create(const char *file, s32 line, Args... args)
        {
            ChunkNode *chunk = GetChunk(file, line);
#else
        T *Create(const char *file, s32 line, Args... args)
        {
            ChunkNode *chunk = GetChunk();
#endif
            ASSERT(chunk, "chunk is null");
            return new(chunk->_data.buff)T(args...);
        }

        void Recover(T *t)
        {
            t->~T();
            Recover((ChunkNode*)t);
        }
    protected:

    private:
#ifdef _DEBUG 
        ChunkNode * GetChunk(const char *file, s32 line)
#else
        ChunkNode * GetChunk()
#endif
        {
            ChunkNode *ret = nullptr;
            if (_chunkRoot == nullptr)
                AllocBlock(1);
            ret = _chunkRoot;
            if (_chunkRoot->_next)
                _chunkRoot->_next->_prev = nullptr;
            _chunkRoot = _chunkRoot->_next;
            ret->_next = nullptr;
            //ret->_prev = nullptr;
            ASSERT(ret&&ret->_data.mark == sizeof(ChunkNode), "chunk invalied");
            ++(ret->_data.parent->_data.count);
            ret->_data.type = IN_USE;

#ifdef _DEBUG
            SafeSprintf(ret->_data.file, sizeof(ret->_data.file), file);
            ret->_data.line = line;
#endif
            return ret;
        }
        void Recover(ChunkNode *chunkNode)
        {
            ASSERT(chunkNode->_data.type == IN_USE && chunkNode->_data.mark == sizeof(ChunkNode), "chunk node invalied");
            ASSERT(chunkNode->_data.parent->_data.count > 0, "chunk list error");

            --(chunkNode->_data.parent->_data.count);
            chunkNode->_data.type = IS_FREE;
            if (chunkNode->_data.parent->_data.count == 0 && _blockCount > block_count)
            {
                RecoverBlock(chunkNode->_data.parent);
                return;
            }
            InsertHead(_chunkRoot, chunkNode);
            return;
        }

        void RecoverBlock(BlockNode *block)
        {
            for (s32 i = 0; i < CHUNK_SIZE; ++i)
            {
                ASSERT(block->_data.chunks[i]._data.type == IS_FREE, "free block, but child chunk is not free type");
                if (&(block->_data.chunks[i]) == _chunkRoot)
                    _chunkRoot = block->_data.chunks[i]._next;
                RemoveNode(&(block->_data.chunks[i]));
            }
            if (_blokRoot == block)
                _blokRoot = block->_next;
            RemoveNode(block);
            SAFE_DELETE(block);
            --_blockCount;
        }

        void AllocBlock(s32 count)
        {
            for (s32 i = 0; i < count; ++i)
            {
                BlockNode *newNode = new BlockNode();
                newNode->_data.count = 0;
                for (s32 j = 0; j < CHUNK_SIZE; ++j)
                {
                    newNode->_data.chunks[j]._next = nullptr;
                    newNode->_data.chunks[j]._prev = nullptr;
                    InsertHead(_chunkRoot, &(newNode->_data.chunks[j]));
                    newNode->_data.chunks[j]._data.parent = newNode;
                    newNode->_data.chunks[j]._data.type = IS_FREE;
                    newNode->_data.chunks[j]._data.mark = sizeof(ChunkNode);
                }
                newNode->_next = nullptr;
                newNode->_prev = nullptr;
                InsertHead(_blokRoot, newNode);
            }
            _blockCount += count;
        }
    private:
        TDynPool(const TDynPool &);
        void operator=(const TDynPool &);
    private:
        s32 _currentAllocs;
        s32 _maxAllocs;
        s32 _blockCount;
        ChunkNode *_chunkRoot;
        BlockNode *_blokRoot;
    };

#ifdef WIN32
#define CREAT_FROM_POOL(pool, ...) pool.Create(__FILE__, __LINE__, __VA_ARGS__)
#else
#define CREAT_FROM_POOL(pool, a...) pool.Create(__FILE__, __LINE__, ##a)
#endif


    template< s32 SIZE >
    class TMemPool
    {
    public:
        TMemPool() : _root(0), _currentAllocs(0), _nAllocs(0), _maxAllocs(0){};
        ~TMemPool() {
            Clear();
        }

        void Clear() {
            while (!_blockPtrs.Empty()) {
                Block* b = _blockPtrs.Pop();
                SAFE_DELETE(b);
            }
            _root = 0;
            _currentAllocs = 0;
            _nAllocs = 0;
            _maxAllocs = 0;
        }

        s32 ItemSize() const	{
            return SIZE;
        }

        s32 CurrentAllocs() const		{
            return _currentAllocs;
        }

        void* Alloc() {
            if (!_root) {
                Block* block = new Block();
                _blockPtrs.Push(block);

                for (s32 i = 0; i<COUNT - 1; ++i) {
                    block->chunk[i].next = &block->chunk[i + 1];
                }
                block->chunk[COUNT - 1].next = 0;
                _root = block->chunk;
            }
            void* result = _root;
            _root = _root->next;

            ++_currentAllocs;
            if (_currentAllocs > _maxAllocs) {
                _maxAllocs = _currentAllocs;
            }
            _nAllocs++;
            return result;
        }

        void Free(void* mem) {
            if (!mem) {
                return;
            }
            --_currentAllocs;
            Chunk* chunk = static_cast<Chunk*>(mem);
#ifdef DEBUG
            memset(chunk, 0xfe, sizeof(Chunk));
#endif
            chunk->next = _root;
            _root = chunk;
        }

        void Trace(const char* name) {
            printf("Mempool %s watermark=%d [%dk] current=%d size=%d nAlloc=%d blocks=%d\n",
                name, _maxAllocs, _maxAllocs*SIZE / 1024, _currentAllocs, SIZE, _nAllocs, _blockPtrs.Size());
        }

        enum { COUNT = (16 * 1024) / SIZE }; // Some compilers do not accept to use COUNT in private part if COUNT is private

    private:
        TMemPool(const TMemPool&);
        void operator=(const TMemPool&);

        union Chunk {
            Chunk*  next;
            char    mem[SIZE];
        };
        struct Block {
            Chunk chunk[COUNT];
        };
        tlib::TDynArray< Block *, 8 > _blockPtrs;
        Chunk* _root;

        s32 _currentAllocs;
        s32 _nAllocs;
        s32 _maxAllocs;
    };

    template< typename T, typename... Args>
    T * CreateFromMemPool(TMemPool<sizeof(T)> &pool, Args... args)
    {
        void *mem = pool.Alloc();
        return new(mem)T(args...);
    }

    template< typename T >
    void RecoverFromMemPool(TMemPool<sizeof(T)> &pool, T *var)
    {
        var->~T();
        pool.Free((void*)var);
    }
}
#endif