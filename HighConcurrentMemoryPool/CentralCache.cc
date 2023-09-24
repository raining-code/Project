#include"CentralCache.h"
CentralCache CentralCache::instance;

CentralCache* CentralCache::getInstance() {
	return &instance;
}

size_t CentralCache::FetchRangeObject(void*& start, void*& end, int index, size_t alignsize, int num) {
	//多个线程可能访问到同一个SpanList
	spanlists[index].mtx.lock();
	Span* span = getSpan(spanlists[index], alignsize);
	assert(span->freelist);
	start = end = span->freelist;
	int realnum = 1;//实际可以取到的个数
	while (--num && NextObj(end)) {
		end = NextObj(end);
		realnum++;
	}
	span->freelist = NextObj(end);
	span->usecount += realnum;//分配出去的内存块的个数增加
	NextObj(end) = nullptr;
	spanlists[index].mtx.unlock();
	//std::cout << realnum << std::endl;
	return realnum;//实际拿到的个数
}

Span* CentralCache::getSpan(SpanList& spanlist, size_t alignsize) {
	//获取一个非空的跨度
	Span* it = spanlist.Begin();
	while (it != spanlist.End()) {
		if (it->freelist) {
			return it;
		}
		it = it->next;
	}
	//表示spanlist中的span的所有内存都分配出去了(此时spanlist处于加锁状态)
	spanlist.mtx.unlock();
	//向PageCache申请大块内存
	PageCache::getInstance()->pagemtx.lock();
	Span* span = PageCache::getInstance()->getKSpan(SizeClass::ApplyPageNumber(alignsize));
	PageCache::getInstance()->pagemtx.unlock();
	//对大块内存进行切分
	span->objsize = alignsize;
	char* start = reinterpret_cast<char*>(span->pageid << PAGE_SHIFT);
	char* end = start + (span->n << PAGE_SHIFT);
	//尾插到freelist,增加CPU缓存的命中率
	span->freelist = start;
	void* tail = span->freelist;
	//error
	//int cnt = 0;
	//while (start < end) {
	//	start += alignsize;
	//	NextObj(tail) = start;
	//	tail = start;
	//	cnt++;
	//}

	//correct
	//int cnt = 1;
	start += alignsize;
	while (start < end) {
		NextObj(tail) = start;
		tail = start;
		start += alignsize;
		//cnt++;
	}
	//std::cout << "cnt=" << cnt << std::endl;
	NextObj(tail) = nullptr;
	spanlist.mtx.lock();
	spanlist.PushFront(span);
	return span;
}

void CentralCache::RecycleMemory(int index, void* start, void* end, size_t mem_size) {
	SpanList& list = spanlists[index];
	list.mtx.lock();
	while (mem_size--) {
		void* next = NextObj(start);
		//将start进行归还
		size_t pageid = (size_t)start >> PAGE_SHIFT;//得到start所在的页
		//根据页找到对应的Span(Span一定在list中)
		//static std::mutex mapmtx;
		//mapmtx.lock();
		Span* span = PageCache::getInstance()->PageNumberToSpan(pageid);//加锁?
		//mapmtx.unlock();
		NextObj(start) = span->freelist;
		span->freelist = start;
		span->usecount--;
		if (span->usecount == 0) {
			list.Erase(span);
			//将span归还给PageCache,span已经从list中移除,归还操作与list无关
			list.mtx.unlock();
			PageCache::getInstance()->pagemtx.lock();
			PageCache::getInstance()->giveBackSpan(span);
			PageCache::getInstance()->pagemtx.unlock();
			list.mtx.lock();
		}
		start = next;
	}
	list.mtx.unlock();
}
