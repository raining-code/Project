//
// Created by cat on 2023/10/18.
//

#ifndef HIGHCONCURRENTMEMORYPOOL_OBJECTPOOL_HPP
#define HIGHCONCURRENTMEMORYPOOL_OBJECTPOOL_HPP

#include <iostream>
#include <algorithm>
#include "SystemPort.h"

using std::cout;
using std::endl;

template<typename T>
class ObjectPool {//定长内存池
public:
    T *New() {
        if (freelist) {
            T *obj = (T *) freelist;
            new(obj)T;//调用构造函数
            freelist = NextObj(freelist);
            return obj;
        }
        size_t size = std::max(sizeof(void *), sizeof(T));
        if (remainbytes < size) {
            memory = (char *) SystemAlloc(128);
            remainbytes = 128 << 13;
            return New();
        }
        T *obj = (T *) memory;
        new(obj)T;
        memory += size;
        remainbytes -= size;
        return obj;
    }

    void Delete(T *obj) {
        obj->~T();//调用析构函数
        NextObj(obj) = freelist;
        freelist = obj;
    }

private:
    char *memory = nullptr;//当前指向的大块内存
    void *freelist = nullptr;//链接还回来的内存
    size_t remainbytes = 0;//当前指向的大块内存还剩多少
};

class Span;

inline ObjectPool<Span> spanpool;

class ThreadCache;

inline ObjectPool<ThreadCache> tcpool;
#endif //HIGHCONCURRENTMEMORYPOOL_OBJECTPOOL_HPP
