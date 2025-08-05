#include "../../include/buffer/asBuffer.h"


asBuffer::asBuffer()
	:m_data(0),m_maxSize(0),m_curSize(0),m_type(BUFFER_DATA_TYPE::BDT_EXTERNAL)
{
}
asBuffer::asBuffer(const asBuffer& other)
	:m_data(0), m_maxSize(0), m_curSize(0), m_type(BUFFER_DATA_TYPE::BDT_EXTERNAL)
{
	Copy(other);
}
asBuffer::asBuffer(ulint size)
	:m_data(0), m_maxSize(0), m_curSize(0), m_type(BUFFER_DATA_TYPE::BDT_EXTERNAL)
{
	Init(size);
}
void asBuffer::operator=(const asBuffer& other)
{
	Copy(other);
}

asBuffer::~asBuffer()
{
	Clear();
}

bool asBuffer::Init(ulint size, BUFFER_DATA_TYPE type)
{
	if (!m_data && size > 0)
	{
		m_data = new char[size];
		m_curSize = 0;
		m_maxSize = size;
		m_type = BUFFER_DATA_TYPE::BDT_NEED_NEW;
		return true;
	}
	return false;
}

bool asBuffer::Init(char* data, ulint size)
{
	if (!m_data && data && size > 0)
	{
		m_data = data;
		m_maxSize = size;
		m_curSize = 0;
		m_type = BUFFER_DATA_TYPE::BDT_EXTERNAL;
		return true;
	}
	return false;
}

ulint asBuffer::Size() const
{
	return m_curSize;
}

ulint asBuffer::MaxSize() const
{
	return m_maxSize;
}

char* asBuffer::Buf() const
{
	return m_data;
}

char* asBuffer::CurrentBuf()
{
	if (m_curSize < m_maxSize)
		return m_data + m_curSize;
	return 0;
}

bool asBuffer::WriteBuf(const void* data, ulint len)
{
	if (len > 0 && m_curSize + len <= m_maxSize)
	{
		memmove(CurrentBuf(), data, len);
		m_curSize += len;
		return true;
	}
	else if (m_type == BUFFER_DATA_TYPE::BDT_NEED_NEW &&
		Resize((m_maxSize + len) * 2, true))
	{
		memcpy(CurrentBuf(), data, len);
		m_curSize += len;
		return true;
	}
	return false;
}

bool asBuffer::Resize(ulint newLen, bool saveData)
{
	if (m_type == BUFFER_DATA_TYPE::BDT_NEED_NEW && newLen > m_maxSize)
	{
		char* pData = new char[newLen];
		if (saveData && m_data && m_curSize > 0 && m_curSize < newLen)
		{
			memmove(pData, m_data, m_curSize);
		}
		else
		{
			m_curSize = 0;
		}
		AS_SAFE_DELETE_ARRAY(m_data)
		m_maxSize = newLen;
		m_data = pData;
		return true;
	}
	return false;
}

bool asBuffer::IsFull() const
{
	return m_curSize >= m_maxSize;
}

i32 asBuffer::BufType() const
{
	return (i32)m_type;
}

bool asBuffer::MoveDataForWard(ulint offset)
{
	if (offset < 0)
		return false;
	if (m_curSize == offset)
	{
		Reset(false);
		return true;
	}
	if (offset < m_curSize)
		return SubData(offset, m_curSize - offset);
	return false;
}

void asBuffer::Reset(bool Zero)
{
	m_curSize = 0;
	if (m_data && m_maxSize && Zero)
		memset(m_data, 0, m_maxSize);
}

ulint asBuffer::RemainSize() const
{
	if (m_curSize <= m_maxSize)
		return m_maxSize - m_curSize;
	return 0;
}

bool asBuffer::SubData(ulint offset, ulint len)
{
	if (m_data && m_curSize > 0)
	{
		if (0 == offset)
		{
			m_curSize = len; // 实际数据没删
			return true;
		}
		else if(offset + len <= m_curSize)
		{
			memmove(m_data, m_data + offset, len);
			m_curSize = len;
			return true;
		}
	}
	return false;
}

bool asBuffer::AddSize(ulint size)
{
	if (m_curSize + size <= m_maxSize)
	{
		m_curSize += size;
		return true;
	}
	return false;
}

bool asBuffer::SubSize(ulint size)
{
	if (size <= m_curSize)
	{
		m_curSize -= size;
		return true;
	}
	return false;
}

bool asBuffer::SetSize(ulint size)
{
	if (size <= m_maxSize)
	{
		m_curSize = size;
		return true;
	}
	return false;
}

void asBuffer::Clear()
{
	m_maxSize = 0;
	m_curSize = 0;
	if (m_type == BUFFER_DATA_TYPE::BDT_NEED_NEW)
		AS_SAFE_DELETE_ARRAY(m_data)
	else
		m_data = nullptr;
	m_type = BUFFER_DATA_TYPE::BDT_NULL;
}

void asBuffer::Copy(const asBuffer& other)
{
	Clear();
	if (other.m_type == BUFFER_DATA_TYPE::BDT_EXTERNAL)
	{
		m_type = other.m_type;
		m_maxSize = other.m_maxSize;
		m_curSize = other.m_curSize;
		m_data = other.m_data;
	}
	else
	{
		m_type = other.m_type;
		m_maxSize = other.m_maxSize;
		m_curSize = other.m_curSize;
		if (m_curSize <= m_maxSize)
		{
			m_data = new char[m_maxSize];
			if (m_curSize > 0)
			{
				memmove(m_data, other.m_data, m_curSize);
			}
		}
	}
}
