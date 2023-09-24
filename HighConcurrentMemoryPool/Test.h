#pragma once
#include"ObjectPool.hpp"
#include"SizeClass.h"
#include"ConcurrentAlloc.h"
#include"ConcurrentFree.h"
#include"RadixTree.h"
#include<vector>
#include<atomic>
#include<thread>
#include<random>
using std::vector;
class Test {
public:
	static void TestRadixTree() {
		ThreeLayerRadixTree tree;
		std::unordered_map<size_t, Span*> umap;
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 1 << 52 - 1);
		for (int i = 0; i < 1000000; i++) {
			Span* span = new Span;
			size_t pageid = dis(gen);
			tree.set(pageid, span);
			umap[pageid] = span;
		}
		for (auto& [k, v] : umap) {
			if (tree.get(k) != v) {
				cout << "error" << endl;
			}
		}
	}
	static void TestObjectPoolInt() {
		constexpr int n = 1000000;
		vector<int*> ptrs;
		ptrs.resize(n * 2);
		clock_t start = clock();
		for (int i = 0; i < n; i++) {
			ptrs[i] = (int*)malloc(sizeof(int));
		}
		for (int i = 0; i < n; i++) {
			free(ptrs[i]);
		}
		clock_t end = clock();
		cout << "malloc cost time is " << end - start << " ms" << endl;
		ObjectPool<int> opool;
		start = clock();
		for (int i = 0; i < n; i++) {
			ptrs[i] = opool.New();
		}
		for (int i = 0; i < n; i++) {
			opool.Delete(ptrs[i]);
		}
		end = clock();
		cout << "objectpool cost time is " << end - start << " ms" << endl;
	}
	static void TestObjectPoolNode() {
		constexpr int n = 1000000;
		vector<TestStruct::TreeNode*> ptrs;
		ptrs.resize(n * 2);
		clock_t start = clock();
		for (int i = 0; i < n; i++) {
			ptrs[i] = new TestStruct::TreeNode;
		}
		for (int i = 0; i < n; i++) {
			delete ptrs[i];
		}
		clock_t end = clock();
		clock_t m_cost_time = end - start;
		ObjectPool<TestStruct::TreeNode> opool;
		start = clock();
		for (int i = 0; i < n; i++) {
			ptrs[i] = opool.New();
		}
		for (int i = 0; i < n; i++) {
			opool.Delete(ptrs[i]);
		}
		end = clock();
		cout << "malloc cost time is " << m_cost_time << " ms" << endl;
		cout << "objectpool cost time is " << end - start << " ms" << endl;
	}
	static void TestAlignIndex() {
		cout << "�ֽ���1���ڵĹ�ϣͰ�±�Ϊ" << SizeClass::HashBucketIndex(1) << endl;
		cout << "�ֽ���129���ڵĹ�ϣͰ�±�Ϊ" << SizeClass::HashBucketIndex(129) << endl;
		cout << "�ֽ���145���ڵĹ�ϣͰ�±�Ϊ" << SizeClass::HashBucketIndex(145) << endl;
	}
	static void TestAlignSize() {
		cout << "8*1024+1�ֽڶ����Ժ�Ĵ�С��" << SizeClass::AlignedSize(8 * 1024 + 1) << endl;
		cout << "13�ֽڶ����Ժ�Ĵ�С��" << SizeClass::AlignedSize(13) << endl;
		cout << "64*1024+1�ֽڶ����Ժ�Ĵ�С��" << SizeClass::AlignedSize(64 * 1024 + 1) << endl;
	}
	static void TestConcurrentAlloc() {
		/*ConcurrentAlloc(6);
		ConcurrentAlloc(8);
		ConcurrentAlloc(1);*/
		for (int i = 0; i < 1024; i++) {
			ConcurrentAlloc(1);
		}
		ConcurrentAlloc(1);
	}
	static void TestConcurrentFree() {
		void* p1 = ConcurrentAlloc(6);
		void* p2 = ConcurrentAlloc(8);
		void* p3 = ConcurrentAlloc(1);
		ConcurrentFree(p1);
		ConcurrentFree(p2);
		ConcurrentFree(p3);
	}
	static void TestBigObjectFree() {
		void* p1 = ConcurrentAlloc(257 * 1024);
		ConcurrentFree(p1);
		void* p2 = ConcurrentAlloc(129 * 8 * 1024);
		ConcurrentFree(p2);
	}
	static void BenchMarkMalloc(int threadnum, int rounds, int counts) {
		std::vector<std::thread> vt(threadnum);
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(1, 256 * 1024);
		std::atomic<size_t> malloctime = { 0 };
		std::atomic<size_t> freetime = { 0 };
		auto lamb = [&] {
			for (int i = 0; i < rounds; i++) {
				vector<void*> mems(counts);
				clock_t begin = clock();
				for (int j = 0; j < counts; j++) {
					mems[j] = malloc(dis(gen) / 1000 + 1);
				}
				clock_t end = clock();
				malloctime += end - begin;
				begin = clock();
				for (int j = 0; j < counts; j++) {
					free(mems[j]);
				}
				end = clock();
				freetime += end - begin;
			}
		};
		for (int i = 0; i < threadnum; i++) {
			vt[i] = std::thread(lamb);
		}
		for (int i = 0; i < threadnum; i++) {
			vt[i].join();
		}
		printf("%u���̲߳�������%u��,ÿһ������%u��,malloc���빲�ƻ���ʱ��Ϊ%ums\n", threadnum, rounds, counts, malloctime.load());
		printf("%u���̲߳����ͷ�%u��,ÿһ���ͷ�%u��,free�ͷŹ��ƻ���ʱ��Ϊ%ums\n", threadnum, rounds, counts, freetime.load());
		printf("%u���߳�������ͷ�һ�����ѵ�ʱ��Ϊ%ums\n", threadnum, freetime.load() + malloctime.load());
	}
	static void BenchMarkConcurrentAlloc(int threadnum, int rounds, int counts) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(1, 256 * 1024);
		std::atomic<size_t> ConcurrentAllocTime = { 0 };
		std::atomic<size_t> ConcurrentFreeTime = { 0 };
		auto lamb = [&] {
			for (int i = 0; i < rounds; i++) {
				vector<void*> mems(counts);
				clock_t begin = clock();
				for (int j = 0; j < counts; j++) {
					mems[j] = ConcurrentAlloc(dis(gen) / 1000 + 1);
				}
				clock_t end = clock();
				ConcurrentAllocTime += end - begin;
				begin = clock();
				for (int j = 0; j < counts; j++) {
					ConcurrentFree(mems[j]);
				}
				end = clock();
				ConcurrentFreeTime += end - begin;
			}
		};
		vector<std::thread> vt(threadnum);
		for (int i = 0; i < threadnum; i++) {
			vt[i] = std::thread(lamb);
		}
		for (int i = 0; i < threadnum; i++) {
			vt[i].join();
		}
		printf("%u���̲߳�������%u��,ÿһ������%u��,ConcurrentAlloc���빲�ƻ���ʱ��Ϊ%ums\n", threadnum, rounds, counts, ConcurrentAllocTime.load());
		printf("%u���̲߳����ͷ�%u��,ÿһ���ͷ�%u��,ConcurrentFree�ͷŹ��ƻ���ʱ��Ϊ%ums\n", threadnum, rounds, counts, ConcurrentFreeTime.load());
		printf("%u���߳�������ͷ�һ�����ѵ�ʱ��Ϊ%ums\n", threadnum, ConcurrentAllocTime.load() + ConcurrentFreeTime.load());
	}
};