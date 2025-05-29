#include "../../include/file/asIniReader.h"

asIniReader::asIniReader()
{
}

asIniReader::~asIniReader()
{
}

bool asIniReader::Open(const char* path)
{
	m_cfg.clear();
	m_file.SetPath(path);
	if (m_file.OpenFile())
	{
		std::string sec;
		std::string key;
		std::string value;
		while (m_file.GetLine())
		{
			char* buf = m_file.GetInnerBuffer().Buf();
			ulint size = ::strlen(buf);
			if (buf[0] == '\n' || buf[0] == '\r' || buf[0] == ';')
			{
				continue;
			}
			else if (buf[0] == '[') // section;
			{
				for (ulint i = 0;i < size;++i)
				{
					if (buf[i] == ']' && i > 0)
					{
						sec.assign(buf + 1, i - 1);
						break;
					}
				}
			}
			else // key = value， 找=符号，去除key，value首尾的空格
			{
				key.clear();
				value.clear();
				bool find = false;
				for (ulint i = 0;i < size;++i)
				{
					if (buf[i] == '=')
					{
						find = true;
					}
					if (find && buf[i] != ' ' && buf[i] != '=') // 找value
					{
						value += buf[i];
					}
					else if(!find && buf[i] != ' ') // 找
					{
						key += buf[i];
					}
				}
				if (find)
				{
					m_cfg[sec].insert(std::make_pair(key, value));
				}
			}
		}
		return true;
	}
	return false;
}

bool asIniReader::HasSection(const char* section)
{
	return m_cfg.find(section) != m_cfg.end();
}

std::string& asIniReader::GetString(const char* section, const char* key, const char* defaultV)
{
	auto itr = m_cfg.find(section);
	if (itr != m_cfg.end())
	{
		auto it = itr->second.find(key);
		if (it != itr->second.end())
		{
			return it->second;
		}
	}
	m_tmp = defaultV;
	return m_tmp;
}

i32 asIniReader::GetInt32(const char* section, const char* key, i32 defaultV)
{
	std::string* pString = &GetString(section, key, " ");
	if (pString == &m_tmp)
	{
		return defaultV;
	}
	return ::atoi(pString->c_str());
}

i64 asIniReader::GetLong(const char* section, const char* key, i64 defaultV)
{
	std::string* pString = &GetString(section, key, " ");
	if (pString == &m_tmp)
	{
		return defaultV;
	}
	return ::atoll(pString->c_str());
}
