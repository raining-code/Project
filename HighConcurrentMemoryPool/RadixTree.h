//
// Created by cat on 2023/10/19.
//

#ifndef HIGHCONCURRENTMEMORYPOOL_RADIXTREE_H
#define HIGHCONCURRENTMEMORYPOOL_RADIXTREE_H

#include "Span.h"
#include "SizeClass.h"

#define BITS_32 19 //2^13=1KB
#define BITS_64 51

class MonoLayerRadixTree {
public:
    MonoLayerRadixTree();

    Span *get(size_t pagenum);

    void set(size_t pagenum, Span *span);

private:
    Span **pagetospan = nullptr;
};

class DoubleLayerRadixTree {
public:
    struct Leaf {
        Span **pagetospan = nullptr;
    };

    DoubleLayerRadixTree();

    Span *get(size_t pagenum);

    void set(size_t pagenum, Span *span);

private:
    static int FirstLayerIndex(size_t pagenum);

    static int SecondLayerIndex(size_t pagenum);

private:
    static constexpr int LAY1 = 5;
    static constexpr int LAY2 = BITS_32 - 5;
    Leaf *leafarray = nullptr;
};

class ThreeLayerRadixTree {
public:
    struct Leaf3 {
        Span **pagetospan = nullptr;
    };
    struct Leaf2 {
        Leaf3 *layer2 = nullptr;
    };

    ThreeLayerRadixTree();

    Span *get(size_t pagenum);

    void set(size_t pagenum, Span *span);

    static int FirstLayerIndex(size_t pagenum);

    static int SecondLayerIndex(size_t pagenum);

    static int ThirdLayerIndex(size_t pagenum);

private:
    Leaf2 *layer1 = nullptr;
    static constexpr int LAY1 = 15;
    static constexpr int LAY2 = 15;
    static constexpr int LAY3 = BITS_64 - LAY1 - LAY2;
};

inline MonoLayerRadixTree::MonoLayerRadixTree() {
    size_t size = sizeof(Span *) * (1 << BITS_32);
    size = SizeClass::AlignedSize(size);
    pagetospan = (Span **) SystemAlloc(size >> PAGE_SHIFT);
    memset(pagetospan, 0, size);
}

inline Span *MonoLayerRadixTree::get(size_t pagenum) {
    assert(pagenum < (1 << BITS_32));
    return pagetospan[pagenum];
}

inline void MonoLayerRadixTree::set(size_t pagenum, Span *span) {
    assert(pagenum < (1 << BITS_32));
    pagetospan[pagenum] = span;
}

inline DoubleLayerRadixTree::DoubleLayerRadixTree() {
    leafarray = new Leaf[1 << LAY1];
}

inline Span *DoubleLayerRadixTree::get(size_t pagenum) {
    int findex = FirstLayerIndex(pagenum);
    if (leafarray[findex].pagetospan == nullptr) {
        return nullptr;
    }
    int sindex = SecondLayerIndex(pagenum);
    return leafarray[findex].pagetospan[sindex];
}

inline void DoubleLayerRadixTree::set(size_t pagenum, Span *span) {
    int findex = FirstLayerIndex(pagenum);
    if (!leafarray[findex].pagetospan) {
        leafarray[findex].pagetospan = new Span *[1 << LAY2]{nullptr};
    }
    int sindex = SecondLayerIndex(pagenum);
    leafarray[findex].pagetospan[sindex] = span;
}

inline int DoubleLayerRadixTree::FirstLayerIndex(size_t pagenum) {
    int index = 0;
    for (int i = LAY2 + LAY1 - 1; i >= LAY2; i--) {
        index = (index * 2) + ((pagenum & ((size_t) 1 << i)) ? 1 : 0);
    }
    return index;
}

inline int DoubleLayerRadixTree::SecondLayerIndex(size_t pagenum) {
    int index = 0;
    for (int i = LAY2 - 1; i >= 0; i--) {
        index = (index * 2) + ((pagenum & ((size_t) 1 << i)) ? 1 : 0);
    }
    return index;
}

/*
static constexpr int FIR_BIT = 15;
static constexpr int SEC_BIT = 15;
static constexpr int THI_BIT = 21;
*/
inline ThreeLayerRadixTree::ThreeLayerRadixTree() {
    layer1 = new Leaf2[1 << LAY1];
}

inline Span *ThreeLayerRadixTree::get(size_t pagenum) {
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

inline void ThreeLayerRadixTree::set(size_t pagenum, Span *span) {
    int findex = FirstLayerIndex(pagenum);
    if (!layer1[findex].layer2) {
        layer1[findex].layer2 = new Leaf3[1 << LAY2];
    }
    int sindex = SecondLayerIndex(pagenum);
    if (layer1[findex].layer2[sindex].pagetospan == nullptr) {
        layer1[findex].layer2[sindex].pagetospan = new Span *[1 << LAY3]{nullptr};
    }
    int tindex = ThirdLayerIndex(pagenum);
    layer1[findex].layer2[sindex].pagetospan[tindex] = span;
}

inline int ThreeLayerRadixTree::FirstLayerIndex(size_t pagenum) {
    int index = 0;
    for (int i = LAY1 + LAY2 + LAY3 - 1; i >= LAY2 + LAY3; i--) {
        index = (index * 2) + (((size_t) 1 << i) & pagenum ? 1 : 0);
    }
    return index;
}

inline int ThreeLayerRadixTree::SecondLayerIndex(size_t pagenum) {
    int index = 0;
    for (int i = LAY2 + LAY3 - 1; i >= LAY3; i--) {
        index = (index * 2) + (((size_t) 1 << i) & pagenum ? 1 : 0);
    }
    return index;
}

inline int ThreeLayerRadixTree::ThirdLayerIndex(size_t pagenum) {
    int index = 0;
    for (int i = LAY3 - 1; i >= 0; i--) {
        index = (index * 2) + (((size_t) 1 << i) & pagenum ? 1 : 0);
    }
    return index;
}

#endif //HIGHCONCURRENTMEMORYPOOL_RADIXTREE_H
