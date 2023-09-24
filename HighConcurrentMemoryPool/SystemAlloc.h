#pragma once
#include<memory>
#ifdef _WIN32
#include<Windows.h>
#endif
//Linux平台
//#ifdef linux
//
//#endif
static inline void* SystemAlloc(size_t kpages) {
	//kpages表示申请的页的数量
	void* ans = nullptr;
#ifdef _WIN32
	ans = VirtualAlloc(0, kpages << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);//一页的大小是8KB
#endif
//#ifdef linux
//	ans=
//#endif
	if (ans == nullptr) {
		throw std::bad_alloc();
	}
	return ans;
}
static inline void*& NextObj(void* obj) {//返回一个指针的大小
	return *(void**)obj;
}