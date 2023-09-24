#include "RadixTree.h"

MonoLayerRadixTree::MonoLayerRadixTree() {
	size_t size = sizeof(Span*) * (1 << BITS_32);
	size = SizeClass::Align(size, 1 << PAGE_SHIFT);
	pagetospan = (Span**)SystemAlloc(size >> PAGE_SHIFT);
	memset(pagetospan, 0, size);
}

Span* MonoLayerRadixTree::get(size_t pagenum) {
	assert(pagenum < (1 << BITS_32));
	return pagetospan[pagenum];
}

void MonoLayerRadixTree::set(size_t pagenum, Span* span) {
	assert(pagenum < (1 << BITS_32));
	pagetospan[pagenum] = span;
}

DoubleLayerRadixTree::DoubleLayerRadixTree() {
	leafarray = new Leaf[1 << LAY1];
}

Span* DoubleLayerRadixTree::get(size_t pagenum) {
	int findex = FirstLayerIndex(pagenum);
	if (leafarray[findex].pagetospan == nullptr) {
		return nullptr;
	}
	int sindex = SecondLayerIndex(pagenum);
	return leafarray[findex].pagetospan[sindex];
}

void DoubleLayerRadixTree::set(size_t pagenum, Span* span) {
	int findex = FirstLayerIndex(pagenum);
	if (!leafarray[findex].pagetospan) {
		leafarray[findex].pagetospan = new Span * [1 << LAY2]{ nullptr };
	}
	int sindex = SecondLayerIndex(pagenum);
	leafarray[findex].pagetospan[sindex] = span;
}

int DoubleLayerRadixTree::FirstLayerIndex(size_t pagenum) {
	int index = 0;
	for (int i = LAY2 + LAY1 - 1; i >= LAY2; i--) {
		index = (index * 2) + ((pagenum & ((size_t)1 << i)) ? 1 : 0);
	}
	return index;
}

int DoubleLayerRadixTree::SecondLayerIndex(size_t pagenum) {
	int index = 0;
	for (int i = LAY2 - 1; i >= 0; i--) {
		index = (index * 2) + ((pagenum & ((size_t)1 << i)) ? 1 : 0);
	}
	return index;
}
/*
static constexpr int FIR_BIT = 15;
static constexpr int SEC_BIT = 15;
static constexpr int THI_BIT = 21;
*/
ThreeLayerRadixTree::ThreeLayerRadixTree() {
	layer1 = new Leaf2[1 << LAY1];
}

Span* ThreeLayerRadixTree::get(size_t pagenum) {
	int findex = FirstLayerIndex(pagenum);
	if (!layer1[findex].layer2) {
		return nullptr;
	}
	int sindex = SecondLayerIndex(pagenum);
	if (!layer1[findex].layer2[sindex].pagetospan) {
		return nullptr;
	}
	int tindex = ThirdLayerIndex(pagenum);
	return layer1[findex].layer2[sindex].pagetospan[tindex];
}

void ThreeLayerRadixTree::set(size_t pagenum, Span* span) {
	int findex = FirstLayerIndex(pagenum);
	if (!layer1[findex].layer2) {
		layer1[findex].layer2 = new Leaf3[1 << LAY2];
	}
	int sindex = SecondLayerIndex(pagenum);
	if (layer1[findex].layer2[sindex].pagetospan == nullptr) {
		layer1[findex].layer2[sindex].pagetospan = new Span * [1 << LAY3]{ nullptr };
	}
	int tindex = ThirdLayerIndex(pagenum);
	layer1[findex].layer2[sindex].pagetospan[tindex] = span;
}

int ThreeLayerRadixTree::FirstLayerIndex(size_t pagenum) {
	int index = 0;
	for (int i = LAY1 + LAY2 + LAY3 - 1; i >= LAY2 + LAY3; i--) {
		index = (index * 2) + (((size_t)1 << i) & pagenum ? 1 : 0);
	}
	return index;
}

int ThreeLayerRadixTree::SecondLayerIndex(size_t pagenum) {
	int index = 0;
	for (int i = LAY2 + LAY3 - 1; i >= LAY3; i--) {
		index = (index * 2) + (((size_t)1 << i) & pagenum ? 1 : 0);
	}
	return index;
}

int ThreeLayerRadixTree::ThirdLayerIndex(size_t pagenum) {
	int index = 0;
	for (int i = LAY3 - 1; i >= 0; i--) {
		index = (index * 2) + (((size_t)1 << i) & pagenum ? 1 : 0);
	}
	return index;
}
