#pragma once
#include"Span.h"
#include"SystemAlloc.h"
#include"PageCache.h"
#include"SizeClass.h"
#include<iostream>
#define NSPANLISTS 208 //CentralCache��ϣͰ�Ľṹ��ThreadCacheһ��
class CentralCache {
public:
	static CentralCache* getInstance();//��ȡ��������
	size_t FetchRangeObject(void*&start, void*&end,int index,size_t alignsize,int num);//��Span�л�ȡһ�������Ķ���
	Span* getSpan(SpanList& spanlist, size_t alignsize);
	void RecycleMemory(int index,void* start,void* end,size_t mem_size);
private:
	CentralCache()=default;
	CentralCache(const CentralCache&) = delete;
	CentralCache& operator=(const CentralCache&) = delete;
	SpanList spanlists[NSPANLISTS];
	static CentralCache instance;//����ģʽ
};