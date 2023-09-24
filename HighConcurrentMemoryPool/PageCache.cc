#include "PageCache.h"
PageCache PageCache::instance;
PageCache* PageCache::getInstance() {
	return &instance;
}

Span* PageCache::getKSpan(int k) {
	if (k >= PAGE_SPANLISTS_NUM) {//ֱ���������
		void* mem = SystemAlloc(k);
		Span* span = spanpool.New();
		span->pageid = reinterpret_cast<size_t>(mem) >> PAGE_SHIFT;
		span->n = k;
		span->isuse = true;
		span->objsize = k << PAGE_SHIFT;
		//spanmap[span->pageid] = span;//������ʼҳ����span��ӳ��
		spanmap.set(span->pageid, span);
		return span;
	}
	if (!spanlists[k].Empty()) {
		Span* span = spanlists[k].PopFront();
		//����ӳ���ϵ
		for (size_t i = 0; i < span->n; i++) {
			spanmap.set(span->pageid + i, span);
			//spanmap[span->pageid + i] = span;
		}
		span->isuse = true;
		return span;
	}
	for (int i = k + 1; i < PAGE_SPANLISTS_NUM; i++) {
		if (!spanlists[i].Empty()) {//�����з�
			Span* nspan = spanlists[i].PopFront();
			Span* ans = spanpool.New();
			ans->pageid = nspan->pageid;
			ans->n = k;
			nspan->pageid += k;
			nspan->n -= k;
			spanlists[nspan->n].PushFront(nspan);
			//����ӳ���ϵ
			for (size_t j = 0; j < ans->n; j++) {
				//spanmap[ans->pageid + i] = ans;
				spanmap.set(ans->pageid + j, ans);
			}
			//������λҳ��ӳ��,���ںϲ�
			//spanmap[nspan->pageid] = nspan;
			//spanmap[nspan->pageid + nspan->n - 1] = nspan;
			spanmap.set(nspan->pageid, nspan);
			spanmap.set(nspan->pageid + nspan->n - 1, nspan);
			ans->isuse = true;
			return ans;
		}
	}
	//�������
	Span* bigspan = spanpool.New();
	void* mem = SystemAlloc(PAGE_SPANLISTS_NUM - 1);
	bigspan->pageid = reinterpret_cast<size_t>(mem) >> PAGE_SHIFT;
	bigspan->n = PAGE_SPANLISTS_NUM - 1;
	spanlists[PAGE_SPANLISTS_NUM - 1].PushFront(bigspan);
	/*spanmap[bigspan->pageid] = bigspan;
	spanmap[bigspan->pageid + bigspan->n - 1] = bigspan;*/
	spanmap.set(bigspan->pageid, bigspan);
	spanmap.set(bigspan->pageid + bigspan->n - 1, bigspan);
	return getKSpan(k);
}

Span* PageCache::PageNumberToSpan(size_t num) {
	//auto it = spanmap.find(num);
	//assert(it != spanmap.end());

	//cout << spanmap.size() << endl;
	//return it->second;
	return spanmap.get(num);
}

void PageCache::giveBackSpan(Span* span) {
	if (span->n >= 129) {//ֱ���ͷŸ�ϵͳ
		void* obj = reinterpret_cast<void*>(span->pageid << PAGE_SHIFT);
		SystemFree(obj);
		spanpool.Delete(span);
		return;
	}
	span->next = span->prev = nullptr;
	span->freelist = nullptr;
	span->isuse = false;
	//����ǰ��ҳ�ĺϲ�
	while (1) {
		size_t previd = span->pageid - 1;
		//auto prev = spanmap.find(previd);
		//if (prev == spanmap.end() || prev->second->isuse == true || prev->second->n + span->n >= PAGE_SPANLISTS_NUM) {
		//	break;
		//}
		//Span* prevspan = prev->second;
		Span* prevspan = spanmap.get(previd);
		if (!prevspan || prevspan->isuse == true || prevspan->n + span->n >= PAGE_SPANLISTS_NUM) {
			break;
		}
		span->pageid = prevspan->pageid;
		span->n += prevspan->n;
		spanlists[prevspan->n].Erase(prevspan);
		spanpool.Delete(prevspan);
	}
	while (1) {
		size_t nextid = span->pageid + span->n;
		//auto next = spanmap.find(nextid);
		//if (next == spanmap.end() || next->second->isuse == true || next->second->n + span->n >= PAGE_SPANLISTS_NUM) {
		//	break;
		//}
		//Span* nextspan = next->second;
		Span* nextspan = spanmap.get(nextid);
		if (!nextspan || nextspan->isuse == true || nextspan->n + span->n >= PAGE_SPANLISTS_NUM) {
			break;
		}
		span->n += nextspan->n;
		spanlists[nextspan->n].Erase(nextspan);
		spanpool.Delete(nextspan);
	}
	spanlists[span->n].PushFront(span);
	//������βҳ��ӳ��,�����´κϲ�
	/*spanmap[span->pageid] = span;
	spanmap[span->pageid + span->n - 1] = span;*/
	spanmap.set(span->pageid, span);
	spanmap.set(span->pageid + span->n - 1, span);
}
