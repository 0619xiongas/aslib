#include "asCFileWriter.h"
#include "asPath.h"
#include "../time/asTime.h"

asCFileWriter::asCFileWriter()
{
	m_app = false;
	m_brief = false;
	m_binary = false;
	m_pFile = NULL;
}
asCFileWriter::~asCFileWriter()
{
	CloseFile();
}

bool asCFileWriter::Init(const char* path, bool app, bool brief, bool binary)
{
	m_path = path;
	m_app = app;
	m_brief = brief;
	m_binary = binary;
	//创建文件夹
	std::string dir = asPath::RemoveFileName(path);
	asPath::MakeDir(dir.c_str());

	if (m_pFile)
	{
		CloseFile();
	}

	// 一直打开
	if (!m_brief)
	{
		if (!OpenFile())
		{
			return false;
		}
	}
	return true;
}

bool asCFileWriter::WriteString(const char* str, bool end, bool time)
{
	if (m_brief)
	{
		OpenFile();
	}
	if (m_pFile == NULL)
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
	if (end)
	{
		buf += "\n";
	}
	::fprintf(m_pFile, buf.c_str());
	if (m_brief)
	{
		CloseFile();
	}
	return true;
}

bool asCFileWriter::WriteBytes(const char* bytes, u32 len)
{

	if (m_brief)
	{
		OpenFile();
	}
	if (m_pFile == NULL)
	{
		return false;
	}
	if (!m_binary)
	{
		return false;
	}
	size_t ret = ::fwrite(bytes, sizeof(char), len, m_pFile);
	if (ret != len)
	{
		CloseFile();
		return false;
	}
	if (m_brief)
	{
		CloseFile();
	}
	return true;
}

bool asCFileWriter::OpenFile()
{
	if (m_pFile)
	{
		return true;
	}
	std::string mode = "";
	if (m_app)
	{
		mode += 'a';
	}
	else
	{
		mode += 'w';
	}
	if (m_binary)
	{
		mode += 'b';
	}

	m_pFile = ::fopen(m_path.c_str(), mode.c_str());
	return m_pFile != NULL;
}

void asCFileWriter::CloseFile()
{
	if (m_pFile)
	{
		::fclose(m_pFile);
		m_pFile = NULL;
	}
}
