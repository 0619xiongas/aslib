#include "asLog.h"
#include <map>
#include "../time/asTime.h"

static std::map<LOGTYPE, std::string> s_strmp = {
	{LOGTYPE::INFO,"[INFO]"},
	{LOGTYPE::DEBUG,"[DEBUG]"},
	{LOGTYPE::WARNING,"[WARNING]"},
	{LOGTYPE::ERROR,"[ERROR]"},
};
asLog::asLog(char sub, const char* path)
	:m_subsection(sub)
{
	memset(m_lastTime, 0, sizeof(char) * 32);
	m_info.GetInfo();
	if (path)
	{
		m_dir = path;
	}
	else
	{
		m_dir = "./";
		m_dir += m_info.GetNameNoExtend();
		m_dir += "_logs";
	}
}

asLog::~asLog()
{
	m_cfw.CloseFile();
}

void asLog::SetSubSection(char type)
{
	m_subsection = type ? 1 : 0;
}

void asLog::Log(LOGTYPE type, const char* str)
{
	FormatString(type, str);
}

void asLog::LogEx(LOGTYPE type, const char* str, ...)
{
	va_list va;
	va_start(va, str);
	FormatStringEx(type, str, va);
	va_end(va);
}

void asLog::FormatString(LOGTYPE type, const char* str)
{
	char ti[11] = { 0 };
	asTime::GetLocalTimeStr(ti);
	if (m_buffer.size())
	{
		m_buffer.clear();
	}
	m_buffer = ti + s_strmp[type] + str;
	SaveToFile();
}

void asLog::FormatStringEx(LOGTYPE type, const char* str, va_list& valist)
{
	char tmp[LOG_STR_MAX * 2] = { 0 };
	if (m_buffer.size())
	{
		m_buffer.clear();
	}
	m_buffer = s_strmp[type];
	i32 ret = vsnprintf(tmp, LOG_STR_MAX * 2 - 1, str, valist);
	if (ret <= 0)
	{
		m_buffer += "vsnprintf error!";
	}
	else
	{
		m_buffer += tmp;
	}
	SaveToFile();
}

void asLog::SaveToFile()
{
	if (m_buffer.empty())
	{
		return;
	}
	CheckMakeFile();
	m_cfw.WriteString(m_buffer.c_str(), true, true);
	m_buffer.clear();
}

void asLog::CheckMakeFile()
{
	bool make = false;
	char ti[32] = { 0 };
	if (m_subsection == 1) //
	{
		asTime::GetLocalTimeStrYMDH(ti);
		if (::strcmp(ti, m_lastTime))
		{
			asTime::GetLocalTimeStrYMDH(m_lastTime);
			make = true;
		}
	}
	else
	{
		asTime::GetLocalTimeStrYMD(ti);
		if (::strcmp(ti, m_lastTime))
		{
			asTime::GetLocalTimeStrYMD(m_lastTime);
			make = true;
		}
	}
	if (make)
	{
		std::string file = m_dir + "/" + m_lastTime + ".log";
		m_cfw.Init(file.c_str(), true, true, false);
	}
}
