//
// Created by cat on 2023/10/19.
//

#ifndef HIGHCONCURRENTMEMORYPOOL_SPAN_H
#define HIGHCONCURRENTMEMORYPOOL_SPAN_H

#include <ctype.h>
#include "FreeList.h"

struct Span {
    size_t pageid = 0;//起始页号
    size_t n = 0;//页的数量
    Span *next = nullptr;
    Span *prev = nullptr;
    FreeList freelist;//切下来的小块内存挂到freelist中
    size_t usecount = 0;
    bool isuse = false;
    size_t objsize = 0;
};
#endif //HIGHCONCURRENTMEMORYPOOL_SPAN_H
