#include "ThreadCache.h"
thread_local ThreadCache* threadTLS = nullptr;
void ThreadCache::DeAlloc(void* obj, size_t size){
	//计算出释放给哪一个哈希桶
	int index = SizeClass::HashBucketIndex(size);
	freelists[index].Push(obj);
	if (freelists[index].Size() >= freelists[index].applycnt) {
		giveBackToCentralCache(freelists[index], SizeClass::AlignedSize(size));//归还一部分内存给CentralCache
	}
}

void* ThreadCache::Alloc(size_t size){
	size_t alignsize = SizeClass::AlignedSize(size);//算出对齐以后的大小
	int index = SizeClass::HashBucketIndex(size);//算出哈希桶的下标
	if (!freelists[index].Empty()) {
		return freelists[index].Pop();
	}
	return FetchFromCentralCache(index, alignsize);
}

void* ThreadCache::FetchFromCentralCache(int index, size_t alignsize){
	//从中心缓存的index号桶中获取一定数量的对象
	int num = min(freelists[index].applycnt, SizeClass::ApplyCnt(alignsize));
	if (num == freelists[index].applycnt) {
		freelists[index].applycnt++;
	}
	void* start = nullptr, * end = nullptr;
	size_t realnum=CentralCache::getInstance()->FetchRangeObject(start, end, index, alignsize,num);
	if (realnum == 1) {
		assert(start == end);
		return start;
	}
	freelists[index].PushRange(NextObj(start), end,realnum-1);//将多出来的内存挂到freelist上
	return start;
}

void ThreadCache::giveBackToCentralCache(FreeList& list, size_t alignsize){
	//list中小块内存的数量=list.applycnt
	int index = SizeClass::HashBucketIndex(alignsize);
	void* start = nullptr, * end = nullptr;
	size_t recycle_size = list.applycnt;
	list.PopRange(start, end, recycle_size);
	//选择将list中的所有内存归还给CentralCache,访问CentralCache的index号桶
	CentralCache::getInstance()->RecycleMemory(index,start, end, recycle_size);
}
