#include "FreeList.h"

void FreeList::Push(void* obj){
	NextObj(obj) = m_freelist;
	m_freelist = obj;
	size++;
}

void* FreeList::Pop(){
	assert(m_freelist);
	void* ans = m_freelist;
	m_freelist = NextObj(m_freelist);
	size--;
	return ans;
}

bool FreeList::Empty(){
	return m_freelist==nullptr;
}

void FreeList::PushRange(void* start, void* end, size_t size){
	//std::cout << "NextObj(end)=" << NextObj(end) << std::endl;
	NextObj(end) = m_freelist;
	m_freelist = start;
	this->size += size;
}

size_t FreeList::Size(){
	return size;
}

void FreeList::PopRange(void*& start, void*& end, size_t popsize){
	assert(size >= popsize);
	size -= popsize;
	start = end = m_freelist;
	while (--popsize) {
		end = NextObj(end);
	}
	m_freelist = NextObj(end);
	NextObj(end) = nullptr;
}
