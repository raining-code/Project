#include "ConcurrentFree.h"

void ConcurrentFree(void* obj) {
	assert(threadTLS);
	//static std::mutex mapmtx;
	size_t pageid = reinterpret_cast<size_t>(obj) >> PAGE_SHIFT;
	//mapmtx.lock();
	Span* span = PageCache::getInstance()->PageNumberToSpan(pageid);
	//mapmtx.unlock();
	size_t size = span->objsize;
	if (size > MAXBYTES) {//´óÓÚ256KB
		PageCache::getInstance()->pagemtx.lock();
		PageCache::getInstance()->giveBackSpan(span); 
		PageCache::getInstance()->pagemtx.unlock();
		return;
	}
	threadTLS->DeAlloc(obj, size);
}
