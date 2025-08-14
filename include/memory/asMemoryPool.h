#ifndef AS_MEMORYPOOL_H
#define AS_MEMORYPOOL_H

#include "../asBaseDefine.h"

template<class T>
class asMemoryPool
{
public:
	asMemoryPool(const asMemoryPool&) = delete;
	asMemoryPool& operator=(const asMemoryPool&) = delete;
	asMemoryPool() :m_blockSize(sizeof(T) < sizeof(BlockNode) ? sizeof(BlockNode) : sizeof(T)),
		m_blockTotalNum(0), m_memoryNum(0)
	{

	}
	asMemoryPool(u32 TNums) :
		m_blockSize(sizeof(T) < sizeof(BlockNode) ? sizeof(BlockNode) : sizeof(T)),
		m_blockNum(TNums), m_blockTotalNum(0), m_memoryNum(0)
	{
		AllocateBlocks(m_blockNum);
	}
	~asMemoryPool()
	{
		MemoryNode* cur = m_memoryList;
		while (cur)
		{
			MemoryNode* next = cur->next;
			::operator delete(cur->memory);
			delete cur;
			cur = next;
		}  
	}
	void Init(u32 TNums)
	{
		m_blockNum = TNums;
		AllocateBlocks(m_blockNum);
	}
	template<typename ... Args>
	T* Allocate(Args&& ...args)
	{
		if (!m_freeList) // 用完了 进行池内存拓展
		{
			AllocateBlocks(m_blockNum / 2);
		}
		void* obj = m_freeList;
		m_freeList = m_freeList->next;
		return new(obj) T(std::forward<Args>(args)...);
	}
	void Destroy(T* obj)
	{
		if (!obj) return ;
		obj->~T();
		BlockNode* node = reinterpret_cast<BlockNode*>(obj);
		node->next = m_freeList;
		m_freeList = node;
	}
private:
	void AllocateBlocks(u32 num)
	{
		void* newChunks = ::operator new(num * m_blockSize);
		MemoryNode* nNode = new MemoryNode{ newChunks,num,m_memoryList };
		m_memoryList = nNode;
		++m_memoryNum;
		
		//存入m_freeList;
		BlockNode* bNode = static_cast<BlockNode*>(newChunks);
		for (u32 i = 0; i < num; ++i)
		{
			BlockNode* next = reinterpret_cast<BlockNode*>(
				static_cast<char*>(newChunks) + i * m_blockSize
				);
			next->next = m_freeList;
			m_freeList = next;
		}
		m_blockTotalNum += num;
	}
private:
	struct BlockNode
	{
		BlockNode* next = nullptr;
	};
	struct MemoryNode
	{
		void*	memory;
		u32		count;
		MemoryNode* next = nullptr;
	};
	BlockNode*		m_freeList = nullptr;   // 块链表
	MemoryNode*		m_memoryList = nullptr;	// 页链表
	u32				m_blockNum;
	u32				m_blockSize;
	u32				m_blockTotalNum;
	u32				m_memoryNum; // 页数
};
#endif