#include "asFileWriter.h"
#include "asPath.h"
#include "../time/asTime.h"

asFileWriter::asFileWriter()
{
	m_app = false;
	m_brief = false;
	m_binary = false;
}

bool asFileWriter::Init(const char* path, bool app, bool brief, bool binary)
{
	m_path = path;
	m_app = app;
	m_brief = brief;
	m_binary = binary;
	//创建文件夹
	std::string dir = asPath::RemoveFileName(path);
	asPath::MakeDir(dir.c_str());
	// 确保文件关闭
	if (m_fs.is_open())
	{
		CloseFile();
	}
	// 一直打开文件流
	if (!m_brief)
	{
		if (!OpenFile())
		{
			return false;
		}
	}
	return true;
}

asFileWriter::~asFileWriter()
{
	CloseFile();
}

bool asFileWriter::WriteString(const char* str, bool end, bool time)
{
	if (m_brief) // 需要重新打开文件
	{
		OpenFile();
	}
	if (!m_fs.is_open())
	{
		return false;
	}
	std::string buf;
	if (time)
	{
		char tstr[TIME_STR_MAX_LEN] = { 0 };
		asTime::GetLocalTimeStr(tstr);
		buf = "[";
		buf += tstr;
		buf += "]";
	}
	buf += str;
	m_fs << buf;
	if (end)
	{
		m_fs << std::endl;
	}
	if (m_brief)
	{
		CloseFile();
	}
	return true;
}

bool asFileWriter::WriteBytes(const char* bytes, u32 len)
{
	if (m_brief) // 需要重新打开文件
	{
		OpenFile();
	}
	if (!m_fs.is_open())
	{
		return false;
	}

	m_fs.write(bytes, len);

	if (m_brief)
	{
		CloseFile();
	}
	return true;
}

bool asFileWriter::OpenFile()
{
	if (m_fs.is_open())
	{
		return true;
	}
	std::ios_base::openmode mode;
	if (m_app)
	{
		mode = std::ios::app;
	}
	else
	{
		mode = std::ios::ate;
	}
	if (m_binary)
	{
		mode |= std::ios::binary;
	}
	m_fs.open(m_path.c_str(), mode);
	return m_fs.is_open();
}

void asFileWriter::CloseFile()
{
	m_fs.clear();
	m_fs.close();
}
