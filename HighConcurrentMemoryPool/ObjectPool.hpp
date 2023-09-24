#pragma once
#include<iostream>
#include<algorithm>
#include"SystemAlloc.h"
using std::cout;
using std::endl;
namespace TestStruct {
	struct TreeNode {
		TreeNode* left = nullptr;
		TreeNode* right = nullptr;
		int val = 0;
		TreeNode() {
			//cout << "TreeNode construct" << endl;
		}
		~TreeNode() {
			//cout << "TreeNode destruct" << endl;
		}
	};
}
template<typename OBJ>
class ObjectPool {
public:
	//定长内存池（对象池）
	OBJ* New() {
		if (m_freelist) {
			OBJ* obj = (OBJ*)m_freelist;
			new(obj)OBJ;//replacement new，调用构造函数
			m_freelist = NextObj(m_freelist);
			return obj;
		}
		size_t size = max(sizeof(void*), sizeof(OBJ));
		if (m_remainbytes < size) {
			m_memory = (char*)SystemAlloc(128);
			m_remainbytes = 128 << 13;
			return New();
		}
		//有足够的空间
		OBJ* obj = (OBJ*)m_memory;
		new(obj)OBJ;//调用构造函数
		m_memory += size;
		m_remainbytes -= size;
		return obj;
	}
	void Delete(OBJ* obj) {
		obj->~OBJ();//调用析构函数
		NextObj(obj) = m_freelist;
		m_freelist = obj;
	}
private:
	char* m_memory = nullptr;//指向大块内存
	void* m_freelist = nullptr;//指向释放的内存块
	size_t m_remainbytes = 0;//大块内存剩余大小
};