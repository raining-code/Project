#pragma once
#include<assert.h>
#include<algorithm>
#define PAGE_SHIFT 13 //2^13=8KB
#define MAXBYTES 256*1024
/*
对齐规则:
字节数					对齐数			哈希桶下标
[1,128]					8bytes			[0,16)
[129,1024]				16bytes			[16,72)
[1025,8*1024]			128bytes		[72,128)
[8*1024+1,64*1024]		1024bytes		[128,184)
[64*1024+1,256*1024]	8*1024bytes		[184,208)
*/
class SizeClass {
public:
	static size_t Align(size_t size, size_t alignnum);
	static int Index(size_t size, int alignshift);//2^alignshift=对齐数,对齐数是2的次方
	//对齐以后的字节数
	static size_t AlignedSize(size_t size);
	//哈希桶下标
	static size_t HashBucketIndex(size_t size);
	static size_t ApplyCnt(size_t alignsize);//申请数量的上限和下限
	static size_t ApplyPageNumber(size_t alignsize);//CentralCache向PageCache进行申请时,需要申请多少页
};