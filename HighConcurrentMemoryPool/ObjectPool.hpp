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
	//�����ڴ�أ�����أ�
	OBJ* New() {
		if (m_freelist) {
			OBJ* obj = (OBJ*)m_freelist;
			new(obj)OBJ;//replacement new�����ù��캯��
			m_freelist = NextObj(m_freelist);
			return obj;
		}
		size_t size = max(sizeof(void*), sizeof(OBJ));
		if (m_remainbytes < size) {
			m_memory = (char*)SystemAlloc(128);
			m_remainbytes = 128 << 13;
			return New();
		}
		//���㹻�Ŀռ�
		OBJ* obj = (OBJ*)m_memory;
		new(obj)OBJ;//���ù��캯��
		m_memory += size;
		m_remainbytes -= size;
		return obj;
	}
	void Delete(OBJ* obj) {
		obj->~OBJ();//������������
		NextObj(obj) = m_freelist;
		m_freelist = obj;
	}
private:
	char* m_memory = nullptr;//ָ�����ڴ�
	void* m_freelist = nullptr;//ָ���ͷŵ��ڴ��
	size_t m_remainbytes = 0;//����ڴ�ʣ���С
};