#pragma once

#include<memory>

#ifdef _WIN32

#include<Windows.h>

#endif

//如果是Linux系统,包含Linux的头文件
static inline void *SystemAlloc(size_t kpages) {
    void *ans = nullptr;//kpages表示申请的页的数量
#ifdef _WIN32
    ans = VirtualAlloc(0, kpages << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#endif
//Linux系统下调用Linux申请内存的系统接口brk/mmap/sbrk
    if (ans == nullptr) {
        throw std::bad_alloc();
    }
    return ans;
}

static inline void SystemFree(void *ptr) {
#ifdef _WIN32
    VirtualFree(ptr, 0, MEM_RELEASE);
#endif
}

static inline void *&NextObj(void *obj) {
    return *(void **) obj;
}