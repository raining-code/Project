cmake_minimum_required(VERSION 3.26)
project(HighConcurrentMemoryPool)

set(CMAKE_CXX_STANDARD 17)

add_executable(HighConcurrentMemoryPool main.cpp
        SystemPort.h
        main.cpp
        ObjectPool.hpp
        UnionTest.h
        FreeList.h
        Span.h
        SpanList.h
        ThreadCache.h
        ThreadCache.h
        ThreadCache.h
        SizeClass.h
        CentralCache.h
        PageCache.h
        RadixTree.h
        RadixTree.h
        RadixTree.h
        Concurrent.h)
