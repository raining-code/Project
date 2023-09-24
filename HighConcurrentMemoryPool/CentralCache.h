#pragma once
#include"Span.h"
#include"SystemAlloc.h"
#include"PageCache.h"
#include"SizeClass.h"
#include<iostream>
#define NSPANLISTS 208 //CentralCache哈希桶的结构与ThreadCache一致
class CentralCache {
public:
	static CentralCache* getInstance();//获取单例对象
	size_t FetchRangeObject(void*&start, void*&end,int index,size_t alignsize,int num);//从Span中获取一定数量的对象
	Span* getSpan(SpanList& spanlist, size_t alignsize);
	void RecycleMemory(int index,void* start,void* end,size_t mem_size);
private:
	CentralCache()=default;
	CentralCache(const CentralCache&) = delete;
	CentralCache& operator=(const CentralCache&) = delete;
	SpanList spanlists[NSPANLISTS];
	static CentralCache instance;//单例模式
};