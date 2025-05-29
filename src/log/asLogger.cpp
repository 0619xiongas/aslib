#include "../../include/log/asLogger.h"
#include <map>
#include "../../include/time/asTime.h"

asLogger::asLogger()
	:m_type(0)
{
	::memset(m_lastTime, 0, sizeof(char) * 32);
	m_info.GetInfo();
}

asLogger::~asLogger()
{
}

bool asLogger::Init(char type, const char* path)
{
	if (path)
	{
		m_dir = path;
	}
	else
	{
		m_dir = "./";
		m_dir += (m_info.m_name + "_logs");
	}
	return true;
}

void asLogger::Log(LOGTYPE type, const char* str)
{
	FormatString(type, str,1);
}

void asLogger::LogEx(LOGTYPE type, const char* str, ...)
{
	va_list va;
	va_start(va, str);
	FormatStringEx(type, str, 1,va);
	va_end(va);
}

void asLogger::PrintConsole(LOGTYPE type, const char* str)
{
	FormatString(type, str, 2);
}

void asLogger::PrintConsoleEx(LOGTYPE type, const char* str, ...)
{
	va_list va;
	va_start(va, str);
	FormatStringEx(type, str, 2, va);
	va_end(va);
}

void asLogger::PrintFile(LOGTYPE type, const char* str)
{
	FormatString(type, str, 3);
}

void asLogger::PrintFileEx(LOGTYPE type, const char* str, ...)
{
	va_list va;
	va_start(va, str);
	FormatStringEx(type, str, 3, va);
	va_end(va);
}

void asLogger::FormatString(LOGTYPE type, const char* str,u32 lv)
{
	char ti[20] = { 0 };
	asTime::GetLocalTimeStr(ti);
	u32 strLen = (u32)::strlen(str);

	char* log = new char[strLen + 28];
	snprintf(log, strLen + 28, "[%s][%s]%s", ti, GetTypeString(type).c_str(), str);
	asMsg msg(AS_THREADMSG_LOGGER, lv, (ulint)log);
	PostMsg(msg);
}

void asLogger::FormatStringEx(LOGTYPE type, const char* str, u32 lv, va_list& vl)
{
	char vaStr[1024] = { 0 };
	i32 vaLen = vsnprintf(vaStr, 1023, str, vl);
	if (vaLen <= 0)
	{
		memcpy(vaStr, "vsnprintf error!\n", 17);
	}
	char ti[20] = { 0 };
	asTime::GetLocalTimeStr(ti);
	u32 strLen = (u32)::strlen(str);
	char* log = new char[1024];
	snprintf(log, 1024, "[%s][%s]%s\n", ti, GetTypeString(type).c_str(), vaStr);
	asMsg msg(AS_THREADMSG_LOGGER, lv, (ulint)log);
	PostMsg(msg);
}

void asLogger::SaveToFile(const char* log)
{
	CheckMakeFile();
	m_cfw.WriteString(log, false, false);
}

void asLogger::CheckMakeFile()
{
	bool make = false;
	char ti[20] = { 0 };
	if (m_type == 1)
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

void asLogger::HandleMessage(asMsg& msg)
{
	if (msg.msgID == AS_THREADMSG_LOGGER)
	{
		char* log = (char*)(msg.params[0]);
		if (msg.subID == 1) // log
		{
			printf(log);
			SaveToFile(log);
		}
		else if (msg.subID == 2)
		{
			printf(log);
		}
		else if (msg.subID == 3)
		{
			SaveToFile(log);
		}
		delete[] log;
	}
}

std::string asLogger::GetTypeString(LOGTYPE type)
{
	std::string str;
	switch (type)
	{
	case TIP:
		str = "TIP";
		break;
	case BUG:
		str = "BUG";
		break;
	case ERR:
		str = "ERR";
		break;
	default:
		str = "TIP";
		break;
	}
	return str;
}
