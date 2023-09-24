#include "Span.h"
ObjectPool<Span> SpanList::spanpool;
SpanList::SpanList(){
	head = spanpool.New();
	head->next = head;
	head->prev = head;
}

void SpanList::Insert(Span* pos, Span* span){
	assert(pos);
	Span* prev = pos->prev;
	prev->next = span;
	span->prev = prev;
	span->next = pos;
	pos->prev = span;
}

void SpanList::Erase(Span* pos){
	assert(pos);
	Span* prev = pos->prev, * next = pos->next;
	prev->next = next;
	next->prev = prev;
}

Span* SpanList::Begin(){
	return head->next;
}

Span* SpanList::End(){
	return head;
}

void SpanList::PushFront(Span* span){
	Insert(Begin(), span);
}

bool SpanList::Empty(){
	return head->next==head;
}

Span* SpanList::PopFront(){
	assert(Empty() == false);
	Span* ans = head->next;
	Erase(ans);
	return ans;
}
