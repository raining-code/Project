#pragma once
#include<cassert>
#include<iostream>
#include"SystemAlloc.h"
class FreeList {
public:
	void Push(void* obj);//���������������
	void* Pop();//������������ȡ������
	bool Empty();
	void PushRange(void* start, void* end,size_t size);//size��ʾPushRange�ĸ���
	size_t Size();
	void PopRange(void*& start, void*& end, size_t popsize);
public:
	size_t applycnt = 1;//�������ĸ���(����ʼ�ķ��������㷨)
private:
	size_t size = 0;
	void* m_freelist = nullptr;
};