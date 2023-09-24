#pragma once
#include<mutex>
#include"Span.h"
#include"SystemAlloc.h"
#include"ObjectPool.hpp"
#include"SystemFree.h"
#include<unordered_map>
#include"RadixTree.h"
#define PAGE_SPANLISTS_NUM 129
#define PAGE_SHIFT 13 //2^13=8KB
class PageCache {
public:
	static PageCache* getInstance();
	Span* getKSpan(int k);
	std::mutex pagemtx;
	Span* PageNumberToSpan(size_t num);
	void giveBackSpan(Span* span);
private:
	SpanList spanlists[PAGE_SPANLISTS_NUM];
	PageCache() = default;
	PageCache(const PageCache&) = delete;
	PageCache& operator=(const PageCache&) = delete;
	static PageCache instance;
#ifdef _WIN64
	ThreeLayerRadixTree spanmap;
#elif _WIN32
	DoubleLayerRadixTree spanmap;
#endif
	//std::unordered_map<size_t, Span*> spanmap;//根据页号找到对应的Span结构
	ObjectPool<Span> spanpool;
};