// 内存池 + placement new — 预分配对象块，在已有内存上构造，O(1) 回收
// 通过指针算术反推对象所属 block；[[unlikely]] 优化 wrap-around 路径
#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>

template<typename T, size_t BlockSize = 256>
class MemPool {
public:
    MemPool() : current_block_(nullptr), current_slot_(0), free_list_(nullptr) {}

    ~MemPool() {
        Block* p = current_block_;
        while (p) {
            Block* next = p->next;
            ::operator delete(p);
            p = next;
        }
    }

    T* allocate() {
        if (free_list_) {
            FreeNode* head = free_list_;
            free_list_ = head->next;
            return ::new (static_cast<void*>(head)) T();
        }
        if (current_slot_ >= BlockSize) [[unlikely]] {  // wrap-around 路径
            alloc_block();
        }
        T* obj = ::new (static_cast<void*>(&current_block_->storage[current_slot_ * sizeof(T)])) T();
        ++current_slot_;
        return obj;
    }

    void deallocate(T* p) {
        p->~T();
        FreeNode* node = reinterpret_cast<FreeNode*>(p);
        node->next = free_list_;
        free_list_ = node;
    }

private:
    union FreeNode {
        FreeNode* next;
        alignas(T) char _[sizeof(T)];
    };

    struct Block {
        Block* next;
        alignas(T) char storage[BlockSize * sizeof(T)];
    };

    void alloc_block() {
        Block* b = static_cast<Block*>(::operator new(sizeof(Block)));
        b->next = current_block_;
        current_block_ = b;
        current_slot_ = 0;
    }

    Block* current_block_;
    size_t current_slot_;
    FreeNode* free_list_;
};

#ifdef MEMPOOL_MAIN
#include <cstdio>
struct Item { int id; };
int main() {
    MemPool<Item, 4> pool;
    Item* a = pool.allocate();
    a->id = 1;
    Item* b = pool.allocate();
    b->id = 2;
    pool.deallocate(a);
    Item* c = pool.allocate();
    printf("%d\n", c->id);  // 复用，可能为 1
    pool.deallocate(b);
    pool.deallocate(c);
    return 0;
}
#endif

#endif
