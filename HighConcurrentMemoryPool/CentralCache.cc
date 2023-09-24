#include"CentralCache.h"
CentralCache CentralCache::instance;

CentralCache* CentralCache::getInstance() {
	return &instance;
}

size_t CentralCache::FetchRangeObject(void*& start, void*& end, int index, size_t alignsize, int num) {
	//����߳̿��ܷ��ʵ�ͬһ��SpanList
	spanlists[index].mtx.lock();
	Span* span = getSpan(spanlists[index], alignsize);
	assert(span->freelist);
	start = end = span->freelist;
	int realnum = 1;//ʵ�ʿ���ȡ���ĸ���
	while (--num && NextObj(end)) {
		end = NextObj(end);
		realnum++;
	}
	span->freelist = NextObj(end);
	span->usecount += realnum;//�����ȥ���ڴ��ĸ�������
	NextObj(end) = nullptr;
	spanlists[index].mtx.unlock();
	//std::cout << realnum << std::endl;
	return realnum;//ʵ���õ��ĸ���
}

Span* CentralCache::getSpan(SpanList& spanlist, size_t alignsize) {
	//��ȡһ���ǿյĿ��
	Span* it = spanlist.Begin();
	while (it != spanlist.End()) {
		if (it->freelist) {
			return it;
		}
		it = it->next;
	}
	//��ʾspanlist�е�span�������ڴ涼�����ȥ��(��ʱspanlist���ڼ���״̬)
	spanlist.mtx.unlock();
	//��PageCache�������ڴ�
	PageCache::getInstance()->pagemtx.lock();
	Span* span = PageCache::getInstance()->getKSpan(SizeClass::ApplyPageNumber(alignsize));
	PageCache::getInstance()->pagemtx.unlock();
	//�Դ���ڴ�����з�
	span->objsize = alignsize;
	char* start = reinterpret_cast<char*>(span->pageid << PAGE_SHIFT);
	char* end = start + (span->n << PAGE_SHIFT);
	//β�嵽freelist,����CPU�����������
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
		//��start���й黹
		size_t pageid = (size_t)start >> PAGE_SHIFT;//�õ�start���ڵ�ҳ
		//����ҳ�ҵ���Ӧ��Span(Spanһ����list��)
		//static std::mutex mapmtx;
		//mapmtx.lock();
		Span* span = PageCache::getInstance()->PageNumberToSpan(pageid);//����?
		//mapmtx.unlock();
		NextObj(start) = span->freelist;
		span->freelist = start;
		span->usecount--;
		if (span->usecount == 0) {
			list.Erase(span);
			//��span�黹��PageCache,span�Ѿ���list���Ƴ�,�黹������list�޹�
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
