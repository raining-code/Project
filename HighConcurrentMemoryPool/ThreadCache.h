//
// Created by cat on 2023/10/19.
//

#ifndef HIGHCONCURRENTMEMORYPOOL_THREADCACHE_H
#define HIGHCONCURRENTMEMORYPOOL_THREADCACHE_H

#include <algorithm>
#include "FreeList.h"
#include "SizeClass.h"
#include "CentralCache.h"

#define THREAD_CACHE_SIZE 208 //thread cache的大小(数组中元素的个数)

class ThreadCache;

inline thread_local ThreadCache *threadTLS = nullptr;//每一个线程一个线程缓存
class ThreadCache {
public:
    void Deallocate(void *obj, size_t size);

    void *Allocate(size_t size);

    void *FetchFromCentralCache(int index, size_t alignsize);

    void giveBackToCentralCache(FreeList &list, size_t alignsize);

private:
    FreeList freelists[THREAD_CACHE_SIZE];
};

inline void ThreadCache::Deallocate(void *obj, size_t size) {
    int index = SizeClass::Index(size);
    freelists[index].push(obj);
    //当前内存块的数量大于下一次向Central Cache申请的内存块的数量
    if (freelists[index].size() >= freelists[index].applycnt) {
        giveBackToCentralCache(freelists[index], SizeClass::AlignedSize(size));
    }
}

inline void *ThreadCache::Allocate(size_t size) {
    size_t alignsize = SizeClass::AlignedSize(size);
    int index = SizeClass::Index(size);
    if (!freelists[index].empty()) {
        return freelists[index].pop();
    }
    //没有内存才到下一层去拿
    return FetchFromCentralCache(index, alignsize);
}

inline void *ThreadCache::FetchFromCentralCache(int index, size_t alignsize) {
    int num = std::min<int>(freelists[index].applycnt, SizeClass::ApplyCnt(alignsize));
    if (num == freelists[index].applycnt) {
        freelists[index].applycnt++;
    }
    void *start = nullptr, *end = nullptr;
    size_t realnum = CentralCache::getInstance()->FetchRangeObject(start, end, index, alignsize, num);
    if (realnum == 1) {
        assert(start == end);
        return start;
    }
    //将多出来的挂回去
    freelists[index].push_range(NextObj(start), end, realnum - 1);
    return start;
}

//list为thread cache中的一个元素,将list中的所有小块内存都还给central cache
inline void ThreadCache::giveBackToCentralCache(FreeList &list, size_t alignsize) {
    int index = SizeClass::Index(alignsize);
    void *start = nullptr, *end = nullptr;
    size_t recycle_size = list.size();
    list.pop_range(start, end, recycle_size);
    CentralCache::getInstance()->RecycleMemory(index, start, end, recycle_size);
}


#endif //HIGHCONCURRENTMEMORYPOOL_THREADCACHE_H
