#include "asFileReader.h"

asFileReader::asFileReader()
{
}

asFileReader::~asFileReader()
{
	Clear();
}

void asFileReader::SetPath(const char* path)
{
	m_path = path;
}

void asFileReader::Clear()
{
	CloseFile();
	//m_buffer.Clear();
}

bool asFileReader::OpenFile(bool b)
{
	if (m_ifs.is_open())
	{
		m_ifs.close();
	}
	if (b)
	{
		m_ifs.open(m_path.c_str(), std::ios::binary);
	}
	else
	{
		m_ifs.open(m_path.c_str());
	}
	return m_ifs.is_open();
}

void asFileReader::CloseFile()
{
	m_ifs.close();
}

ulint asFileReader::GetFileSize()
{
	if (m_ifs.is_open())
	{
		m_ifs.seekg(0, std::ios::end);
		ulint size = (ulint)m_ifs.tellg();
		m_ifs.seekg(0, std::ios::beg);
		return size;
	}
	return 0;
}

bool asFileReader::GetLine()
{
	if (m_ifs.good())
	{
		if (m_buffer.Buf())
		{
			m_ifs.getline(m_buffer.Buf(), m_buffer.MaxSize());
			return true;
		}
		else if (!m_buffer.Buf() && CreateBuffer())
		{
			m_ifs.getline(m_buffer.Buf(), m_buffer.MaxSize());
			return true;
		}
	}
	return false;
}

bool asFileReader::ReadAll()
{
	ulint size = GetFileSize();
	if (!m_buffer.Buf())
	{
		if (!CreateBuffer())
			return false;
	}
	if (size > m_buffer.MaxSize())
	{
		m_buffer.Resize(size, false);
	}
	if (size > m_buffer.MaxSize())
	{
		return false;
	}
	if (!m_ifs.is_open())
	{
		return false;
	}
	m_ifs.read(m_buffer.Buf(), size);
	m_buffer.SetSize(size);
	return false;
}

asBuffer& asFileReader::GetInnerBuffer()
{
	return m_buffer;
}

bool asFileReader::CreateBuffer()
{
	ulint size = GetFileSize();
	if (size > 0)
	{
		if (m_buffer.Init(size + 4))
		{
			m_buffer.Reset(true);
			return true;
		}
	}
	return false;
}
