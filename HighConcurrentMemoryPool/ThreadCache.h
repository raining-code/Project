#pragma once
#include"FreeList.h"
#include"SizeClass.h"
#include"CentralCache.h"
#include<algorithm>
#define NFREELISTS 208 //线程缓存中有208个哈希桶
#define MAX_BYTES 256*1024 //线程缓存中允许申请的最大内存大小为256KB
class ThreadCache {
public:
	void DeAlloc(void* obj, size_t size);//释放对象
	void* Alloc(size_t size);
	void* FetchFromCentralCache(int index, size_t alignsize);
	void giveBackToCentralCache(FreeList& list, size_t alignsize);
private:
	FreeList freelists[NFREELISTS];//线程缓存是哈希桶结构
};
extern thread_local ThreadCache* threadTLS;
