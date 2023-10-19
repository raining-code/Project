//
// Created by cat on 2023/10/19.
//

#ifndef HIGHCONCURRENTMEMORYPOOL_CENTRALCACHE_H
#define HIGHCONCURRENTMEMORYPOOL_CENTRALCACHE_H
#define CENTRAL_CACHE_SIZE 208 //central cache大小与thread cache相同

#include "SpanList.h"
#include "SizeClass.h"
#include "PageCache.h"

class CentralCache {
public:
    static CentralCache *getInstance();

    size_t FetchRangeObject(void *&start, void *&end, int index, size_t alignsize, int num);

    Span *getSpan(SpanList &spanlist, size_t alignsize);

    void RecycleMemory(int index, void *start, void *end, size_t size);

private:
    CentralCache() = default;

    CentralCache(const CentralCache &) = delete;

    CentralCache &operator=(const CentralCache &) = delete;

    SpanList spanlists[CENTRAL_CACHE_SIZE];
};


inline CentralCache *CentralCache::getInstance() {
    static CentralCache instance;
    return &instance;//懒汉模式
}

inline size_t CentralCache::FetchRangeObject(void *&start, void *&end, int index, size_t alignsize, int num) {
    //多个线程访问同一个spanlist需要加锁
    spanlists[index].mtx.lock();
    Span *span = getSpan(spanlists[index], alignsize);
    assert(!span->freelist.empty());
    int size = span->freelist.size();//存放的内存块的个数
    int realnum = std::min<int>(num, size);
    span->freelist.pop_range(start, end, realnum);
    span->usecount += realnum;//只要usecount不为0,span一定被使用,usecount为0,结合isuse判断是否可以归还
    spanlists[index].mtx.unlock();
    return realnum;
}

inline Span *CentralCache::getSpan(SpanList &spanlist, size_t alignsize) {
    Span *it = spanlist.begin();
    while (it != spanlist.end()) {
        if (!it->freelist.empty()) {
            return it;
        }
        it = it->next;
    }
    //表示Centra cache中该下标的SpanList已经没有可用的Span
    spanlist.mtx.unlock();
    //找PageCache要Span
    PageCache::getInstance()->pagemtx.lock();
    Span *span = PageCache::getInstance()->getKSpan(SizeClass::ApplyPageNumber(alignsize));
    PageCache::getInstance()->pagemtx.unlock();
    //切要到的Span
    span->objsize = alignsize;
    char *start = reinterpret_cast<char *>(span->pageid << PAGE_SHIFT);
    char *end = start + (span->n << PAGE_SHIFT);
    while (start < end) {
        span->freelist.push(start);
        start += alignsize;
    }
    spanlist.mtx.lock();
    spanlist.push_front(span);
    return span;
}

inline void CentralCache::RecycleMemory(int index, void *start, void *end, size_t size) {
    SpanList &list = spanlists[index];//将内存还到list中
    /*list中有多个Span,需要确定还给哪一个Span*/
    list.mtx.lock();
    while (size--) {
        void *next = NextObj(start);//先记录下一个
        size_t pageid = (size_t) start >> PAGE_SHIFT;
        Span *span = PageCache::getInstance()->PageNumberToSpan(pageid);//根据页号得到管理它的span,使用基数树不需要加锁
        span->freelist.push(start);
        span->usecount--;
        if (span->usecount == 0) {
            list.erase(span);
            list.mtx.unlock();
            PageCache::getInstance()->pagemtx.lock();
            PageCache::getInstance()->giveBackSpan(span);//将小的页合并成大的页
            PageCache::getInstance()->pagemtx.unlock();
            list.mtx.lock();
        }
        start = next;
    }
    assert(start == nullptr);
    list.mtx.unlock();
}

#endif //HIGHCONCURRENTMEMORYPOOL_CENTRALCACHE_H
