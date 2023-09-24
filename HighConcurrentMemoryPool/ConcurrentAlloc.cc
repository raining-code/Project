#include"ConcurrentAlloc.h"
void* ConcurrentAlloc(size_t size) {
	if (threadTLS == nullptr) {
		static ObjectPool<ThreadCache> tcpool;
		static std::mutex poolmtx;
		poolmtx.lock();
		threadTLS = tcpool.New();
		poolmtx.unlock();
	}
	if (size > MAXBYTES) {//大于256KB,向PageCache进行申请
		int npage = SizeClass::AlignedSize(size);
		npage >>= PAGE_SHIFT;
		PageCache::getInstance()->pagemtx.lock();
		Span* span = PageCache::getInstance()->getKSpan(npage);
		PageCache::getInstance()->pagemtx.unlock();
		span->objsize = npage << PAGE_SHIFT;
		return reinterpret_cast<void*>(span->pageid << PAGE_SHIFT);
	}
	return threadTLS->Alloc(size);
}
