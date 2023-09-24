#pragma once
#include<memory>
#ifdef _WIN32
#include<Windows.h>
#endif
//Linuxƽ̨
//#ifdef linux
//
//#endif
static inline void SystemFree(void* ptr) {
#ifdef _WIN32
	VirtualFree(ptr, 0, MEM_RELEASE);
#endif
//#ifdef linux

//#endif
}