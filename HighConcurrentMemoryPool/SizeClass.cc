#include "SizeClass.h"

size_t SizeClass::Align(size_t size, size_t alignnum) {
	return ((size + alignnum - 1) & ~(alignnum - 1));//仅当alignnum为2的整数次方时有效
}

int SizeClass::Index(size_t size, int alignshift) {
	return ((size + (1 << alignshift) - 1) >> alignshift) - 1;
}

size_t SizeClass::AlignedSize(size_t size) {
	if (size <= 128) {
		return Align(size, 8);
	}
	else if (size <= 1024) {
		return Align(size, 16);
	}
	else if (size <= 8 * 1024) {
		return Align(size, 128);
	}
	else if (size <= 64 * 1024) {
		return Align(size, 1024);
	}
	else if (size <= 256 * 1024) {
		return Align(size, 8 * 1024);
	}
	else {//大于256KB,按照一页进行对齐
		return Align(size, 8 * 1024);
	}
}

size_t SizeClass::HashBucketIndex(size_t size) {
	static constexpr int indexarray[] = { 16,56,56,56 };
	if (size <= 128) {
		return Index(size, 3);//2^3=8
	}
	else if (size <= 1024) {
		return Index(size - 128, 4) + indexarray[0];//2^4=16
	}
	else if (size <= 8 * 1024) {
		return Index(size - 1024, 7) + indexarray[0] + indexarray[1];//2^7=128
	}
	else if (size <= 64 * 1024) {
		return Index(size - 8 * 1024, 10) + indexarray[0] + indexarray[1] + indexarray[2];;//2^10=1024
	}
	else if (size <= 256 * 1024) {
		return Index(size - 64 * 1024, 13) + indexarray[0] + indexarray[1] + indexarray[2] + indexarray[3];//2^13=8*1024
	}
	else {
		assert(false);
	}
}

size_t SizeClass::ApplyCnt(size_t alignsize){
	int num = MAXBYTES / alignsize;
	if (num > 512) {
		return 512;
	}
	else if (num < 2) {
		return 2;
	}
	else {
		return num;
	}
}

size_t SizeClass::ApplyPageNumber(size_t alignsize){
	int num = ApplyCnt(alignsize);
	return std::max<int>(1, (num * alignsize) >> PAGE_SHIFT);
}
