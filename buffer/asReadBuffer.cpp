#include "asReadBuffer.h"

asReadBuffer::asReadBuffer()
	:m_pos(0)
{
}

asReadBuffer::asReadBuffer(const char* source, ulint size)
	:m_pos(0)
{
	m_buf.Init(const_cast<char*>(source), size);
}

asReadBuffer::~asReadBuffer()
{
}
void asReadBuffer::Set(const char* source, ulint size)
{
	m_buf.Init(const_cast<char*>(source), size);
	m_pos = 0;
}
void asReadBuffer::Skip(ulint size)
{
	if (size + m_pos > m_buf.Size())
		return;
	m_pos += size;
}
bool asReadBuffer::IsEmpty()
{
	return m_buf.Size() == 0;
}
bool asReadBuffer::ReadString(std::string& str)
{
	u32 len = 0;
	if (ReadU32(len))
	{
		str.clear();
		if (len == 0)
		{
			return true; // ¿ÕÊý¾Ý
		}
		else
		{
			str.assign(m_buf.Buf() + m_pos, len);
			Skip(len);
			return true;
		}
	}
	return false;
}
u32 asReadBuffer::ReadString(char* data, u32 size)
{
	u32 len = 0;
	if (ReadU32(len))
	{
		if (len == 0)
		{
			data[0] = 0;
			return 0;
		}
		else if (len < size)
		{
			_Read(data, len);
			data[len] = 0;
			return len;
		}
	}
	return 0;
}
u32 asReadBuffer::ReadStruct(void* data, u32 size)
{
	u32 len = 0;
	if (ReadU32(len))
	{
		if (len == 0)
		{
			return 0;
		}
		else if (len <= size)
		{
			_Read(data, len);
			return len;
		}
	}
	return 0;
}
bool asReadBuffer::_Read(void* data, ulint size)
{
	if (size + m_pos > m_buf.MaxSize())
		return false;
	memcpy(data, m_buf.Buf() + m_pos, size);
	m_pos += size;
	return true;
}
const char* asReadBuffer::_Read(ulint size)
{
	if (size + m_pos > m_buf.MaxSize())
		return 0;
	m_pos += size;
	return m_buf.Buf() + m_pos - size;
}
