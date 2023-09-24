#pragma once
#include"SizeClass.h"
#include"SystemAlloc.h"
#include"Span.h"
#include"ObjectPool.hpp"
#define BITS_32 19 //适用于32位平台
#define BITS_64 51
class MonoLayerRadixTree {
public:
	//单层基数树仅适用于32位平台
	MonoLayerRadixTree();
	Span* get(size_t pagenum);
	void set(size_t pagenum, Span* span);
private:
	Span** pagetospan = nullptr;
};
class DoubleLayerRadixTree {
public:
	//二层基数树也仅适用于32位平台
	struct Leaf {//第2层
		Span** pagetospan=nullptr;
	};
	DoubleLayerRadixTree();
	Span* get(size_t pagenum);
	void set(size_t pagenum, Span* span);
private:
	int FirstLayerIndex(size_t pagenum);
	int SecondLayerIndex(size_t pagenum);
private:
	static constexpr int LAY1 = 5;
	static constexpr int LAY2 = BITS_32-5;
	Leaf* leafarray = nullptr;//第一层
};
class ThreeLayerRadixTree {//64位平台
public:
	struct Leaf3 {//第3层
		Span** pagetospan = nullptr;
	};
	struct Leaf2 {//第2层
		Leaf3* layer2 = nullptr;
	};
	ThreeLayerRadixTree();
	Span* get(size_t pagenum);
	void set(size_t pagenum, Span* span);
	int FirstLayerIndex(size_t pagenum);
	int SecondLayerIndex(size_t pagenum);
	int ThirdLayerIndex(size_t pagenum);
private:
	Leaf2* layer1 = nullptr;//第一层
	static constexpr int LAY1 = 15;
	static constexpr int LAY2 = 15;
	static constexpr int LAY3 = BITS_64-LAY1-LAY2;
};