//
// Created by cat on 2023/10/19.
//

#ifndef HIGHCONCURRENTMEMORYPOOL_SPANLIST_H
#define HIGHCONCURRENTMEMORYPOOL_SPANLIST_H


#include <mutex>
#include "ObjectPool.hpp"
#include "Span.h"

//SpanList为CentralCache中的元素
class SpanList {
public:
    SpanList();

    void insert(Span *pos, Span *span);

    void erase(Span *pos);

    Span *begin();

    Span *end();

    void push_front(Span *span);

    bool empty();

    Span *pop_front();

private:
    Span *head = nullptr;
public:
    std::mutex mtx;//不同线程访问同一个SpanList需要加锁
};

inline SpanList::SpanList() {
    head = spanpool.New();
    head->next = head;
    head->prev = head;
}

//在pos之前插入元素
inline void SpanList::insert(Span *pos, Span *span) {
    assert(pos);
    Span *prev = pos->prev;
    prev->next = span;
    span->prev = prev;
    span->next = pos;
    pos->prev = span;
}

inline void SpanList::erase(Span *pos) {
    assert(pos);
    Span *prev = pos->prev, *next = pos->next;
    prev->next = next;
    next->prev = prev;
    //不需要spanpoll.Delete(pos),erase的含义是从SpanList中取出一个Span进行使用
}

inline Span *SpanList::begin() {
    return head->next;
}

inline Span *SpanList::end() {
    return head;
}

//在begin之前插入
inline void SpanList::push_front(Span *span) {
    insert(begin(), span);
}

inline bool SpanList::empty() {
    return head->next == head;
}

inline Span *SpanList::pop_front() {
    assert(empty() == false);
    Span *ans = head->next;
    erase(ans);
    return ans;
}

#endif //HIGHCONCURRENTMEMORYPOOL_SPANLIST_H
