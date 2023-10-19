//
// Created by cat on 2023/10/19.
//

#ifndef HIGHCONCURRENTMEMORYPOOL_CONCURRENT_H
#define HIGHCONCURRENTMEMORYPOOL_CONCURRENT_H

#include "ThreadCache.h"

inline void *ConcurrentAllocate(size_t size) {
    if (threadTLS == nullptr) {
        static std::mutex poolmtx;
        poolmtx.lock();
        threadTLS = tcpool.New();
        poolmtx.unlock();
    }
    if (size > THREAD_CACHE_MAX_BYTES) {//大于256KB
        int npage = SizeClass::AlignedSize(size);
        npage >>= PAGE_SHIFT;
        PageCache::getInstance()->pagemtx.lock();
        Span *span = PageCache::getInstance()->getKSpan(npage);
        PageCache::getInstance()->pagemtx.unlock();
        span->objsize = npage << PAGE_SHIFT;
        return reinterpret_cast<void *>(span->pageid << PAGE_SHIFT);
    }
    return threadTLS->Allocate(size);
}

inline void ConcurrentDeallocate(void *obj) {
    assert(threadTLS);
    size_t pageid = reinterpret_cast<size_t>(obj) >> PAGE_SHIFT;
    Span *span = PageCache::getInstance()->PageNumberToSpan(pageid);
    size_t size = span->objsize;
    if (size > THREAD_CACHE_MAX_BYTES) {
        PageCache::getInstance()->pagemtx.lock();
        PageCache::getInstance()->giveBackSpan(span);
        PageCache::getInstance()->pagemtx.unlock();
        return;
    }
    threadTLS->Deallocate(obj, size);
}

#endif //HIGHCONCURRENTMEMORYPOOL_CONCURRENT_H
