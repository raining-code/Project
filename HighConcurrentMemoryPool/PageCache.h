//
// Created by cat on 2023/10/19.
//

#ifndef HIGHCONCURRENTMEMORYPOOL_PAGECACHE_H
#define HIGHCONCURRENTMEMORYPOOL_PAGECACHE_H

#include "SpanList.h"
#include "ObjectPool.hpp"
#include "SizeClass.h"
#include "RadixTree.h"

#define PAGE_CACHE_SIZE 129//表示page cache最多管理128页

class PageCache {
public:
    static PageCache *getInstance();

    Span *getKSpan(int k);

    Span *PageNumberToSpan(size_t num);

    void giveBackSpan(Span *span);

public:
    std::mutex pagemtx;
private:
    SpanList spanlists[PAGE_CACHE_SIZE];

    PageCache() = default;

    PageCache(const PageCache &) = delete;

    PageCache &operator=(const PageCache &) = delete;

#ifdef _WIN64
    ThreeLayerRadixTree spanmap;
#elif _WIN32
    DoubleLayerRadixTree spanmap;
#endif
};

inline PageCache *PageCache::getInstance() {
    static PageCache instance;
    return &instance;
}

inline Span *PageCache::getKSpan(int k) {
    if (k >= PAGE_CACHE_SIZE) {//直接向OS申请
        void *mem = SystemAlloc(k);
        Span *span = spanpool.New();
        span->pageid = reinterpret_cast<size_t>(mem) >> PAGE_SHIFT;
        span->n = k;
        span->isuse = true;
        span->objsize = k << PAGE_SHIFT;
        spanmap.set(span->pageid, span);
        return span;
    }
    if (!spanlists[k].empty()) {
        Span *span = spanlists[k].pop_front();
        for (size_t i = 0; i < span->n; i++) {
            spanmap.set(span->pageid + i, span);
        }
        span->isuse = true;
        return span;
    }
    //往后找更大的页拆分
    for (int i = k + 1; i < PAGE_CACHE_SIZE; i++) {
        if (!spanlists[i].empty()) {
            Span *ispan = spanlists[i].pop_front();
            Span *ans = spanpool.New();
            ans->pageid = ispan->pageid;
            ans->n = k;
            ispan->pageid += k;
            ispan->n -= k;
            spanlists[ispan->n].push_front(ispan);
            for (size_t j = 0; j < ans->n; j++) {
                spanmap.set(ans->pageid + j, ans);
            }
            spanmap.set(ispan->pageid, ispan);
            spanmap.set(ispan->pageid + ispan->n - 1, ispan);
            ans->isuse = true;
            return ans;
        }
    }
    //向堆申请128页内存
    Span *bigspan = spanpool.New();
    void *mem = SystemAlloc(PAGE_CACHE_SIZE - 1);
    bigspan->pageid = reinterpret_cast<size_t>(mem) >> PAGE_SHIFT;
    bigspan->n = PAGE_CACHE_SIZE - 1;
    spanlists[PAGE_CACHE_SIZE - 1].push_front(bigspan);
    spanmap.set(bigspan->pageid, bigspan);
    spanmap.set(bigspan->pageid + bigspan->n - 1, bigspan);
    return getKSpan(k);
}

inline Span *PageCache::PageNumberToSpan(size_t num) {
    return spanmap.get(num);
}

inline void PageCache::giveBackSpan(Span *span) {
    if (span->n >= 129) {//还给OS
        void *obj = reinterpret_cast<void *>(span->pageid << PAGE_SHIFT);
        SystemFree(obj);
        spanpool.Delete(span);
        return;
    }
    span->next = span->prev = nullptr;
    span->freelist.clear();
    span->isuse = false;
    span->objsize = 0;
    //向前合并
    while (1) {
        size_t previd = span->pageid - 1;
        Span *prevspan = spanmap.get(previd);
        if (!prevspan || prevspan->isuse || prevspan->n + span->n >= PAGE_CACHE_SIZE) {
            break;
        }
        span->pageid = prevspan->pageid;
        span->n += prevspan->n;
        spanlists[prevspan->n].erase(prevspan);
        spanpool.Delete(prevspan);
    }
    while (1) {
        size_t nextid = span->pageid + span->n;
        Span *nextspan = spanmap.get(nextid);
        if (!nextspan || nextspan->isuse || nextspan->n + span->n >= PAGE_CACHE_SIZE) {
            break;
        }
        span->n += nextspan->n;
        spanlists[nextspan->n].erase(nextspan);
        spanpool.Delete(nextspan);
    }
    spanlists[span->n].push_front(span);
    spanmap.set(span->pageid, span);
    spanmap.set(span->pageid + span->n - 1, span);
}

#endif //HIGHCONCURRENTMEMORYPOOL_PAGECACHE_H
