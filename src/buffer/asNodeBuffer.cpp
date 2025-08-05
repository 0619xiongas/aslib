#include "../../include/buffer/asNodeBuffer.h"

asNodeBuffer::asNodeBuffer()
	:m_count(0),m_offset(0)
{
}

asNodeBuffer::asNodeBuffer(const asNodeBuffer& other)
{
	CopyFrom(other);
}

asNodeBuffer::~asNodeBuffer()
{
	Clear();
}

void asNodeBuffer::operator=(const asNodeBuffer& other)
{
	CopyFrom(other);
}

bool asNodeBuffer::Init(ulint size)
{
	return m_buf.Init(size);
}

bool asNodeBuffer::FormatFrom(char* buf, ulint len)
{
	char flag = 0;
	ulint count = 0;
	ulint offset = 0;
	while(flag == 0)
	{
		ulint *nLen = (ulint*)(buf + offset);
		++count;
		offset += sizeof(ulint) + *nLen;
		if(offset == len)
		{
			flag = 1;
			break;
		}
		else if(offset > len)
		{
			flag = 2;
			break;
		}
	}
	if(flag == 1)
	{
		if(!m_buf.Init(buf, len))
			return false;
		m_count = count;
		return m_buf.SetSize(len);
	}
	else if(flag == 2)
	{
		return false;
	}
	return false;
}

ulint asNodeBuffer::Count() const
{
	return m_count;
}

ulint asNodeBuffer::Size() const
{
	return m_buf.Size();
}

ulint asNodeBuffer::RemainSize() const
{
	return m_buf.RemainSize();
}

const char* asNodeBuffer::Buf() const
{
	return m_buf.Buf();
}

void asNodeBuffer::Empty()
{
	m_count = 0;
	m_offset = 0;
	m_buf.Reset();
}

asNodeBuffer::asNBIterator asNodeBuffer::Begin()
{
	asNBIterator itr;
	if (m_buf.Size() > 0)
	{
		itr.pData = m_buf.Buf() + sizeof(ulint);
		itr.size = *reinterpret_cast<ulint*>(m_buf.Buf()); // 前面8个字节就是第一个node数据的长度
		itr.count = 1;
	}
	return itr;
}

bool asNodeBuffer::Next(asNBIterator& itr)
{
	if (itr.count >= m_count || m_buf.Size() == 0)
	{
		itr.Clear();
		return false;
	}
	ulint size = itr.size;
	if (size > m_buf.Size())
	{
		itr.Clear();
		return false;
	}
	itr.size = *reinterpret_cast<ulint*>(itr.pData + size);
	itr.pData = itr.pData + sizeof(ulint) + size;
	++itr.count;
	return true;
}

void asNodeBuffer::Clear()
{
	m_count = 0;
	m_offset = 0;
	m_buf.Clear();
}

void asNodeBuffer::StartWrite()
{
	m_offset = m_buf.Size();
	// 占位
	const ulint writeSize = 0;
	if (!m_buf.WriteBuf(&writeSize, sizeof(ulint)))
	{
		m_offset = AS_INVAILD_U64;
	}
}

bool asNodeBuffer::Write(const void* buf, ulint len)
{
	if (m_offset == AS_INVAILD_U64)
		return false;

	return m_buf.WriteBuf(buf,len);
}

bool asNodeBuffer::WriteSerialize‌(const void* buf, ulint len)
{
	if (m_offset == AS_INVAILD_U64)
		return false;

	if (len + sizeof(u32) + m_buf.Size() > m_buf.MaxSize())
		return false;

	u32 tLen = (u32)len;
	if (m_buf.WriteBuf(&tLen, sizeof(u32)))
	{
		return m_buf.WriteBuf(buf, len);
	}
	return false;
}

void asNodeBuffer::EndWrite()
{
	if (m_offset != AS_INVAILD_U64)
	{
		ulint total = m_buf.Size();
		ulint write = total - m_offset;
		if (write >= sizeof(ulint) && write <= total)
		{
			++m_count;
			ulint* pWrite = reinterpret_cast<ulint*>(m_buf.Buf() + m_offset);
			*pWrite = write - sizeof(ulint);
		}
		// 防止连续调用此函数
		m_offset = AS_INVAILD_U64;
	}
}

void asNodeBuffer::CopyFrom(const asNodeBuffer& other)
{
	Clear();
	m_count = other.m_count;
	m_buf = other.m_buf;
}
