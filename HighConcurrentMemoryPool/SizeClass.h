//
// Created by cat on 2023/10/19.
//

#ifndef HIGHCONCURRENTMEMORYPOOL_SIZECLASS_H
#define HIGHCONCURRENTMEMORYPOOL_SIZECLASS_H

#include<assert.h>
#include<algorithm>

#define THREAD_CACHE_MAX_BYTES (256*1024) //thread cache支持<=256KB内存的申请
#define PAGE_SHIFT 13 //2^13=1页

class SizeClass {
public:
    static size_t AlignedSize(size_t size);

    static size_t Index(size_t size);

    static size_t ApplyCnt(size_t alignsize);

    static size_t ApplyPageNumber(size_t alignsize);

private:
    static int _index(size_t size, int alignshift);

    static size_t _alignsize(size_t size, size_t alignnum);
};

//_alignsize(2,8):2字节按照8字节对齐->8字节
inline size_t SizeClass::_alignsize(size_t size, size_t alignnum) {
    return ((size + alignnum - 1) & ~(alignnum - 1));
}

//_index:工具函数,用来辅助计算下标
inline int SizeClass::_index(size_t size, int alignshift) {
    return ((size + (1 << alignshift) - 1) >> alignshift) - 1;
}

/*
对齐规则:
[1,128]					8bytes			[0,16)
[129,1024]				16bytes			[16,72)
[1025,8*1024]			128bytes		[72,128)
[8*1024+1,64*1024]		1024bytes		[128,184)
[64*1024+1,256*1024]	8*1024bytes		[184,208)
*/
inline size_t SizeClass::AlignedSize(size_t size) {
    if (size <= 128) {
        return _alignsize(size, 8);
    } else if (size <= 1024) {
        return _alignsize(size, 16);
    } else if (size <= 8 * 1024) {
        return _alignsize(size, 128);
    } else if (size <= 64 * 1024) {
        return _alignsize(size, 1024);
    } else if (size <= 256 * 1024) {
        return _alignsize(size, 8 * 1024);
    } else {//大于256KB,都按照一页对齐
        return _alignsize(size, 8 * 1024);
    }
}

//计算在thread cache的freelists的哪一个下标
inline size_t SizeClass::Index(size_t size) {
    static constexpr int indexarray[] = {16, 56, 56, 56};
    if (size <= 128) {
        return _index(size, 3);//2^3=8
    } else if (size <= 1024) {
        return _index(size - 128, 4) + indexarray[0];//2^4=16
    } else if (size <= 8 * 1024) {
        return _index(size - 1024, 7) + indexarray[0] + indexarray[1];//2^7=128
    } else if (size <= 64 * 1024) {
        return _index(size - 8 * 1024, 10) + indexarray[0] + indexarray[1] + indexarray[2];;//2^10=1024
    } else if (size <= 256 * 1024) {
        return _index(size - 64 * 1024, 13) + indexarray[0] + indexarray[1] + indexarray[2] +
               indexarray[3];//2^13=8*1024
    } else {
        assert(false);
    }
}

//thread cache向Central cache一次要多少
inline size_t SizeClass::ApplyCnt(size_t alignsize) {
    int num = THREAD_CACHE_MAX_BYTES / alignsize;
    if (num > 512) {
        return 512;
    } else if (num < 2) {
        return 2;
    } else {
        return num;
    }
}

//central cache向page cache要多少页
inline size_t SizeClass::ApplyPageNumber(size_t alignsize) {
    int num = ApplyCnt(alignsize);
    return std::max<int>(1, (num * alignsize) >> PAGE_SHIFT);
}

#endif //HIGHCONCURRENTMEMORYPOOL_SIZECLASS_H
