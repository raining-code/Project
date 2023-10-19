//
// Created by cat on 2023/10/19.
//

#ifndef HIGHCONCURRENTMEMORYPOOL_FREELIST_H
#define HIGHCONCURRENTMEMORYPOOL_FREELIST_H

#include <cassert>
#include "SystemPort.h"

class FreeList {
public:
    void push(void *obj);

    void *pop();

    bool empty();

    void push_range(void *start, void *end, size_t size);

    size_t size();

    void pop_range(void *&start, void *&end, size_t size);

    void clear();

public:
    size_t applycnt = 1;//下一次向Central cache申请的内存块数量
private:
    size_t count = 0;//表示FreeList下挂的内存块的个数
    void *freelist = nullptr;
};

inline void FreeList::push(void *obj) {
    NextObj(obj) = freelist;
    freelist = obj;
    count++;
}

inline void *FreeList::pop() {
    assert(freelist);
    void *ans = freelist;
    freelist = NextObj(freelist);
    count--;
    return ans;
}

inline bool FreeList::empty() {
    return freelist == nullptr;
}

inline void FreeList::push_range(void *start, void *end, size_t size) {
    NextObj(end) = freelist;
    freelist = start;
    count += size;
}

inline size_t FreeList::size() {
    return count;
}

inline void FreeList::pop_range(void *&start, void *&end, size_t size) {
    assert(count >= size);
    count -= size;
    start = end = freelist;
    while (--size) {
        end = NextObj(end);
    }
    freelist = NextObj(end);
    NextObj(end) = nullptr;
}

inline void FreeList::clear() {
    freelist = nullptr;
    count=0;
    applycnt=1;
}

#endif //HIGHCONCURRENTMEMORYPOOL_FREELIST_H
