#pragma once
#include<cassert>
#include<iostream>
#include"SystemAlloc.h"
class FreeList {
public:
	void Push(void* obj);//插入对象到自由链表
	void* Pop();//从自由链表中取出对象
	bool Empty();
	void PushRange(void* start, void* end,size_t size);//size表示PushRange的个数
	size_t Size();
	void PopRange(void*& start, void*& end, size_t popsize);
public:
	size_t applycnt = 1;//申请对象的个数(慢开始的反馈调节算法)
private:
	size_t size = 0;
	void* m_freelist = nullptr;
};