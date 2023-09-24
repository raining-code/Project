#pragma once
#include<assert.h>
#include<mutex>
#include"ObjectPool.hpp"
struct Span {//������ҳΪ��λ���ڴ�
	size_t pageid = 0;//�ڴ����ʼҳ��
	size_t n = 0;//ҳ������
	Span* next = nullptr;
	Span* prev = nullptr;
	void* freelist = nullptr;//�����зֺõ��ڴ����������
	size_t usecount = 0;//�кõ��ڴ�,��ThreadCacheʹ�õ�����
	bool isuse = false;//�Ƿ�CentralCacheʹ��
	size_t objsize=0;//����Ĵ�С
};
class SpanList {//CentralCache�е�ÿһ��Ԫ����SpanList
public:
	SpanList();
	void Insert(Span* pos, Span* span);//posλ��֮ǰ����һ��Ԫ��
	void Erase(Span* pos);//ɾ��posλ�õ�Ԫ��
	Span* Begin();
	Span* End();
	void PushFront(Span* span);//ͷ��һ�����
	bool Empty();
	Span* PopFront();
private:
	Span* head = nullptr;
	static ObjectPool<Span> spanpool;
public:
	std::mutex mtx;//����̷߳��ʵ�CentralCache�е�ͬһ��SpanListʱ��Ҫ����
};