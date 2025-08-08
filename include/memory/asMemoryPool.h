#ifndef AS_MEMORYPOOL_H
#define AS_MEMORYPOOL_H

#include "../asBaseDefine.h"
template<u32 BlockSize, u32 NumBlocks>
class asMemoryPool
{
public:
	asMemoryPool(const asMemoryPool& ) = delete;
	asMemoryPool& operator=(const asMemoryPool& ) = delete;
	asMemoryPool()
	~asMemoryPool()
private:
	union Block
	{
		Block* next;
		char data[BlockSize];
	};
	Block* m_dataList;

};

template<class T>
class asMemoryPool
{
public:

private:
	static constexpr u32 m_blockSize = 
	struct ListNode
	{
		ListNode* next;
	};
	ListNode* 	m_freeList;
	char* 		m_memory;
	u32			m_blockCount;
}
#endif