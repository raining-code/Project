#pragma once
#include<memory>
#ifdef _WIN32
#include<Windows.h>
#endif
//Linuxƽ̨
//#ifdef linux
//
//#endif
static inline void* SystemAlloc(size_t kpages) {
	//kpages��ʾ�����ҳ������
	void* ans = nullptr;
#ifdef _WIN32
	ans = VirtualAlloc(0, kpages << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);//һҳ�Ĵ�С��8KB
#endif
//#ifdef linux
//	ans=
//#endif
	if (ans == nullptr) {
		throw std::bad_alloc();
	}
	return ans;
}
static inline void*& NextObj(void* obj) {//����һ��ָ��Ĵ�С
	return *(void**)obj;
}