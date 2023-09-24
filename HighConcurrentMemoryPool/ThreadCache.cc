#include "ThreadCache.h"
thread_local ThreadCache* threadTLS = nullptr;
void ThreadCache::DeAlloc(void* obj, size_t size){
	//������ͷŸ���һ����ϣͰ
	int index = SizeClass::HashBucketIndex(size);
	freelists[index].Push(obj);
	if (freelists[index].Size() >= freelists[index].applycnt) {
		giveBackToCentralCache(freelists[index], SizeClass::AlignedSize(size));//�黹һ�����ڴ��CentralCache
	}
}

void* ThreadCache::Alloc(size_t size){
	size_t alignsize = SizeClass::AlignedSize(size);//��������Ժ�Ĵ�С
	int index = SizeClass::HashBucketIndex(size);//�����ϣͰ���±�
	if (!freelists[index].Empty()) {
		return freelists[index].Pop();
	}
	return FetchFromCentralCache(index, alignsize);
}

void* ThreadCache::FetchFromCentralCache(int index, size_t alignsize){
	//�����Ļ����index��Ͱ�л�ȡһ�������Ķ���
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
	freelists[index].PushRange(NextObj(start), end,realnum-1);//����������ڴ�ҵ�freelist��
	return start;
}

void ThreadCache::giveBackToCentralCache(FreeList& list, size_t alignsize){
	//list��С���ڴ������=list.applycnt
	int index = SizeClass::HashBucketIndex(alignsize);
	void* start = nullptr, * end = nullptr;
	size_t recycle_size = list.applycnt;
	list.PopRange(start, end, recycle_size);
	//ѡ��list�е������ڴ�黹��CentralCache,����CentralCache��index��Ͱ
	CentralCache::getInstance()->RecycleMemory(index,start, end, recycle_size);
}
