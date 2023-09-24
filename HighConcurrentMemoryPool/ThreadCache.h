#pragma once
#include"FreeList.h"
#include"SizeClass.h"
#include"CentralCache.h"
#include<algorithm>
#define NFREELISTS 208 //�̻߳�������208����ϣͰ
#define MAX_BYTES 256*1024 //�̻߳������������������ڴ��СΪ256KB
class ThreadCache {
public:
	void DeAlloc(void* obj, size_t size);//�ͷŶ���
	void* Alloc(size_t size);
	void* FetchFromCentralCache(int index, size_t alignsize);
	void giveBackToCentralCache(FreeList& list, size_t alignsize);
private:
	FreeList freelists[NFREELISTS];//�̻߳����ǹ�ϣͰ�ṹ
};
extern thread_local ThreadCache* threadTLS;
