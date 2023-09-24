#pragma once
#include<assert.h>
#include<mutex>
#include"ObjectPool.hpp"
struct Span {//管理以页为单位的内存
	size_t pageid = 0;//内存的起始页号
	size_t n = 0;//页的数量
	Span* next = nullptr;
	Span* prev = nullptr;
	void* freelist = nullptr;//管理切分好的内存的自由链表
	size_t usecount = 0;//切好的内存,给ThreadCache使用的数量
	bool isuse = false;//是否被CentralCache使用
	size_t objsize=0;//对象的大小
};
class SpanList {//CentralCache中的每一个元素是SpanList
public:
	SpanList();
	void Insert(Span* pos, Span* span);//pos位置之前插入一个元素
	void Erase(Span* pos);//删除pos位置的元素
	Span* Begin();
	Span* End();
	void PushFront(Span* span);//头插一个跨度
	bool Empty();
	Span* PopFront();
private:
	Span* head = nullptr;
	static ObjectPool<Span> spanpool;
public:
	std::mutex mtx;//多个线程访问到CentralCache中的同一个SpanList时需要加锁
};