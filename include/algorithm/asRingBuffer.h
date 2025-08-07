#ifndef AS_RINGBUFFER_H
#define AS_RINGBUFFER_H

#include "../asBaseDefine.h"

template<class T>
class asRingBuffer
{
public:
	asRingBuffer(const asRingBuffer&) = delete;
	asRingBuffer& operator=(const asRingBuffer&) = delete;
	asRingBuffer(u64 size)
	{
		// 确保是2^x
		m_capacity = 1;
		while (m_capacity < size)
		{
			m_capacity <<= 1;
		}
		m_buffer = new T * [m_capacity];
		m_read = 0;
		m_write = 0;
		memset(m_buffer, 0, sizeof(T*) * m_capacity)
	}
	~asRingBuffer()
	{
		if (m_buffer)
		{
			delete[] m_buffer;
			m_buffer = nullptr;
		}
	}
	inline bool IsEmpty() const
	{
		return m_write == m_read;
	}
	inline bool IsFull() const
	{
		return (m_write - m_read) >= m_capacity;
	}
	inline u64 Size() const
	{
		return m_write - m_read;
	}
	inline bool Push(T* t)
	{
		if (IsFull())
			return false;
		u64 mask = m_capacity - 1;
		m_buffer[m_write & mask] = t;
		++m_write;
		return true;
	}
	inline void PushForce(T* t)
	{
		if (IsFull())
		{
			++m_read;
		}
		u64 mask = m_capacity - 1;
		m_buffer[m_write & mask] = t;
		++m_write;
	}
	inline T* Pop()
	{
		if (IsEmpty())
			return nullptr;
		u64 mask = m_capacity - 1;
		T* t = m_buffer[m_read & mask];
		++m_read;
		return t;
	}
private:
	u64		m_capacity;
	u64		m_write; // 写指针 不是写索引 
	u64		m_read; // 读指针  不是读索引
	T**		m_buffer;
};
#endif