#include "../../include/tools/asStringUtils.hpp"

namespace astronaut{
std::string Int32ToString(i32 a)
{
	char num[16] = { 0 };
	::snprintf(num, 16, "%d", a);
	num[15] = 0;
	return num;
}
std::string Int64ToString(i64 a)
{
	char num[24] = { 0 };
	::snprintf(num, 24, "%lld", a);
	num[23] = 0;
	return num;
}
std::string floatToStr(f32 a)
{
	char num[32] = { 0 };
	snprintf(num, 32, "%f", a);
	num[31] = 0;
	return num;
}
i32 String2Int32(const char* str)
{
	return ::atoi(str);
}
u32 String2UInt32(const char* str)
{
	u32 a = 0;
	sscanf(str, "%u", &a);
	return a;
}
u32 HexString2UInt32(const char* str)
{
	u32 a = 0;
	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
		str += 2;
	}
	sscanf(str, "%x", &a);
	return a;
}
i64 String2Int64(const char* str)
{
	return ::atoll(str);
}
u64 String2UInt64(const char* str)
{
	u64 a = 0;
	sscanf(str, "%llu", &a);
	return a;
}
f32 String2Float(const char* str)
{
	return (float)::atof(str);
}
void StringSplit(const std::string& strs, char flag, std::vector<std::string>& strlist)
{
	strlist.clear();
	if (strs.size() > 1)
	{
		std::string tmp;
		size_t start = 0;
		for (size_t i = 0;i < strs.size();++i)
		{
			if (strs[i] == flag)
			{
				tmp = strs.substr(start, i - start);
				if (tmp.size() > 0)
				{
					strlist.push_back(tmp);
				}
				start = i + 1;
			}
			else if (i == strs.size() - 1)
			{
				tmp.clear();
				tmp = strs.substr(start, strs.size() - start);
				if (tmp.size() > 0)
				{
					strlist.push_back(tmp);
				}
			}
		}
	}
	else
	{
		strlist.push_back(strs);
	}
}
void StringSplit(const std::string& strs, const char* flag, std::vector<std::string>& strlist)
{
	if (!flag)
	{
		return;
	}
	strlist.clear();
	std::string tmpFlag(flag);
	if (strs.size() > 1 && !tmpFlag.empty())
	{
		size_t start = 0;
		std::string sub;
		for (size_t i = 0;i < strs.size();++i)
		{
			if (i == strs.size() - 1)
			{
				sub.clear();
				sub = strs.substr(start, strs.size() - start);
				if (!sub.empty())
				{
					strlist.push_back(sub);
				}
			}
			else if(strs[i] == tmpFlag[0])
			{
				bool cmp = true;
				for (size_t k = 1;k < tmpFlag.size();++k)
				{
					if (i + k >= strs.size())
					{
						cmp = false;
						break;
					}
					if (strs[i + k] != tmpFlag[k])
					{
						cmp = false;
						break;
					}
				}
				if (cmp)
				{
					sub = strs.substr(start, i - start);
					if (!sub.empty())
					{
						strlist.push_back(sub);
					}
					start = i + tmpFlag.size();
				}
			}
		}
	}
	else
	{
		strlist.push_back(strs);
	}
}
void StringSplit(const std::string& strs, const char flag1, const char flag2, std::vector<std::vector<std::string>>& lists)
{
	lists.clear();
	std::vector<std::string> vec;
	StringSplit(strs, flag1, vec);
	for (size_t i = 0;i < vec.size();++i)
	{
		std::vector<std::string> vec1;
		StringSplit(vec[i], flag2, vec1);
		lists.push_back(vec1);
	}
}
}
