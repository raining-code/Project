#pragma once
#include"SizeClass.h"
#include"SystemAlloc.h"
#include"Span.h"
#include"ObjectPool.hpp"
#define BITS_32 19 //������32λƽ̨
#define BITS_64 51
class MonoLayerRadixTree {
public:
	//�����������������32λƽ̨
	MonoLayerRadixTree();
	Span* get(size_t pagenum);
	void set(size_t pagenum, Span* span);
private:
	Span** pagetospan = nullptr;
};
class DoubleLayerRadixTree {
public:
	//���������Ҳ��������32λƽ̨
	struct Leaf {//��2��
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
	Leaf* leafarray = nullptr;//��һ��
};
class ThreeLayerRadixTree {//64λƽ̨
public:
	struct Leaf3 {//��3��
		Span** pagetospan = nullptr;
	};
	struct Leaf2 {//��2��
		Leaf3* layer2 = nullptr;
	};
	ThreeLayerRadixTree();
	Span* get(size_t pagenum);
	void set(size_t pagenum, Span* span);
	int FirstLayerIndex(size_t pagenum);
	int SecondLayerIndex(size_t pagenum);
	int ThirdLayerIndex(size_t pagenum);
private:
	Leaf2* layer1 = nullptr;//��һ��
	static constexpr int LAY1 = 15;
	static constexpr int LAY2 = 15;
	static constexpr int LAY3 = BITS_64-LAY1-LAY2;
};