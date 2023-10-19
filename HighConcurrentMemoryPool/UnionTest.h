//
// Created by cat on 2023/10/18.
//


#ifndef HIGHCONCURRENTMEMORYPOOL_UNIONTEST_H
#define HIGHCONCURRENTMEMORYPOOL_UNIONTEST_H

#include "Concurrent.h"
#include <iostream>
#include <vector>
#include <thread>
#include <random>

using std::vector;
using std::cout;
using std::endl;

//=========================================================
struct Foo {
public:
    Foo() : num(new int[5]{0}) {
        cout << "Foo()" << endl;
    }

    ~Foo() {
        cout << "~Foo()" << endl;
        delete[] num;
        num = nullptr;
    }

private:
    int *num = nullptr;
};

inline void TestObjectPool() {
    ObjectPool<Foo> pool;
    vector<Foo *> vec;
    for (int i = 0; i < 10; ++i) {
        Foo *f = pool.New();
        vec.push_back(f);
    }
    for (auto p: vec) {
        cout << p << ' ';
    }
    cout << endl;
    for (int i = 0; i < 10; ++i) {
        pool.Delete(vec[i]);
    }
    cout << endl;
    Foo *f = pool.New();
    cout << f << endl;
    pool.Delete(f);
}

//=========================================================
inline void TestFreeList() {
    char *mem = (char *) malloc(100);
    char *start = mem, *end = mem;
    int size = 5;
    while (--size) {
        NextObj(end) = end + 8;
        end += 8;
    }
    FreeList flist;
    flist.push_range(start, end, 5);
    cout << flist.empty() << endl;
    cout << flist.size() << endl;
    void *a, *b;
    flist.pop_range(a, b, 5);
    cout << flist.empty() << endl;
    cout << flist.size() << endl;
    free(mem);
}

//=========================================================
inline void TestAllocate() {
    ThreadCache t;
    void *p1 = t.Allocate(1);
    void *p2 = t.Allocate(1);
    void *p3 = t.Allocate(1);
    t.Deallocate(p1, 1);
    t.Deallocate(p2, 1);
    t.Deallocate(p3, 1);
}

//=========================================================
inline void TestBigAllocate() {
    void *p = ConcurrentAllocate(257 * 1024);
    ConcurrentDeallocate(p);
}

//=========================================================
inline void BenchMark1() {
    static std::mt19937 mt;
    static std::uniform_int_distribution<int> dis(255 * 1024, 129 * 8 * 1024);
    constexpr int round = 10000;
    vector<size_t> sizes;
    vector<void *> vec;
    int begin = clock();
    for (int i = 0; i < round; i++) {
        sizes.push_back(dis(mt));
        vec.push_back(malloc(sizes.back()));
    }
    int end = clock();
    cout << "malloc cost time:" << end - begin << "ms" << endl;
    begin = clock();
    for (int i = 0; i < round; ++i) {
        free(vec[i]);
    }
    end = clock();
    cout << "free cost time:" << end - begin << "ms" << endl;
    begin = clock();
    for (int i = 0; i < round; i++) {
        vec[i] = ConcurrentAllocate(sizes[i]);
    }
    end = clock();
    cout << "ConcurrentAllocate cost time:" << end - begin << "ms" << endl;
    begin = clock();
    for (int i = 0; i < round; ++i) {
        ConcurrentDeallocate(vec[i]);
    }
    end = clock();
    cout << "ConcurrentDeallocate cost time:" << end - begin << "ms" << endl;
}

inline void BenchMark2() {
    int threadnum = 50;
    vector<std::thread> threads(threadnum);
    int begin = clock();
    for (int i = 0; i < threadnum; ++i) {
        threads[i] = std::thread([]() {
            int round = 10000;
            vector<void *> vec(round);
            std::mt19937 mt;
            std::uniform_int_distribution<int> dis(1, 256 * 1024);
            for (int i = 0; i < round; i++) {
//                vec[i] = ConcurrentAllocate(dis(mt));
                vec[i] = ConcurrentAllocate(i + 1);
            }
            for (int i = 0; i < round; ++i) {
                ConcurrentDeallocate(vec[i]);
            }
        });
    }
    for (auto &t: threads) {
        t.join();
    }
    int end = clock();
    cout << "ConcurrentAllocate cost time:" << end - begin << "ms" << endl;
    begin = clock();
    for (int i = 0; i < threadnum; ++i) {
        threads[i] = std::thread([]() {
            int round = 10000;
            vector<void *> vec(round);
            std::mt19937 mt;
            std::uniform_int_distribution<int> dis(1, 256 * 1024);
            for (int i = 0; i < round; i++) {
//                vec[i] = malloc(dis(mt));
                vec[i] = malloc(i + 1);
            }
            for (int i = 0; i < round; ++i) {
                free(vec[i]);
            }
        });
    }
    for (auto &t: threads) {
        t.join();
    }
    end = clock();
    cout << "malloc cost time:" << end - begin << "ms" << endl;
}

inline void RunTest() {
    BenchMark2();
}
//=========================================================

#endif //HIGHCONCURRENTMEMORYPOOL_UNIONTEST_H
