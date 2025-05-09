#include "asWriteBuffer.h"


asWriteBuffer::asWriteBuffer()
{
}
asWriteBuffer::asWriteBuffer(const asWriteBuffer& wb)
{
	CopyFrom(wb);
}
asWriteBuffer::asWriteBuffer(u32 size, BUFFER_DATA_TYPE type)
{
	Set(size, type);
}
asWriteBuffer::asWriteBuffer(char* data, u32 size)
{
	Set(data, size);
}
asWriteBuffer::~asWriteBuffer()
{
	Clear();
}
void asWriteBuffer::operator=(const asWriteBuffer& other)
{
	CopyFrom(other);
}
bool asWriteBuffer::Set(u32 size, BUFFER_DATA_TYPE type)
{
	if (!m_buffer.Buf())
	{
		m_buffer.Init(size, type);
		return true;
	}
	return false;
}
bool asWriteBuffer::Set(char* data, u32 size)
{
	if (!m_buffer.Buf())
	{
		m_buffer.Init(data, size);
		return true;
	}
	return false;
}
bool asWriteBuffer::ReSize(ulint size)
{
	return m_buffer.Resize(size, false);
}
void asWriteBuffer::Clear()
{
	m_buffer.Clear();
}
bool asWriteBuffer::WriteString(std::string str)
{
	return WriteStruct(str.c_str(), str.size());
}
bool asWriteBuffer::WriteString(const char* data, ulint size)
{
	return WriteStruct(data, size);
}
bool asWriteBuffer::WriteStruct(const void* data, ulint size)
{
	if (size <= 0)
		return false;
	bool ret = false;
	u32 len = (u32)size;
	if (len + sizeof(u32) + m_buffer.Size() > m_buffer.MaxSize())
		return false;
	ret = m_buffer.WriteBuf(&len, sizeof(u32));
	if (ret)
	{
		ret = m_buffer.WriteBuf(data, len);
	}
	return ret;
}
void asWriteBuffer::CopyFrom(const asWriteBuffer& wb)
{
	m_buffer = wb.m_buffer;
}
bool asWriteBuffer::_Write(const void* data, ulint size)
{
	return m_buffer.WriteBuf(data, size);
}
