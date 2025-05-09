#ifndef AS_STRINGUTILS_H
#define AS_STRINGUTILS_H
#include "../asBaseDefine.h"
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>

namespace astronaut {
template<typename T>
u32 StringCopy(T* dst, u32 dstlen, const T* src, u32 srclen)
{
	u32 len = dstlen > srclen ? srclen : (dstlen - 1);
	::memcpy(dst, src, len);
	dst[len] = 0;
	return len;
}

template<typename T>
std::string Num2String(T t)
{
	std::ostringstream os;
	if (os << t)
		return os.str();
	return "";
}
template<typename T>
void Num2String(T t, char* out, u32 len)
{
	std::ostringstream os;
	if (os << t)
	{
		std::string& str = os.str();
		StringCopy(out, len, str.c_str(), str.length());
	}
}
inline std::string Int32ToString(i32 a)
{
	char num[16] = { 0 };
	::snprintf(num, 16, "%d", a);
	num[15] = 0;
	return num;
}
inline std::string Int64ToString(i64 a)
{
	char num[24] = { 0 };
	::snprintf(num, 24, "%lld", a);
	num[23] = 0;
	return num;
}
inline std::string floatToStr(f32 a)
{
	char num[32] = { 0 };
	snprintf(num, 32, "%f", a);
	num[31] = 0;
	return num;
}
inline i32 String2Int32(const char* str)
{
	return ::atoi(str);
}
inline u32 String2UInt32(const char* str)
{
	u32 a = 0;
	sscanf(str, "%u", &a);
	return a;
}
inline i64 String2Int64(const char* str)
{
	return ::atoll(str);
}
inline u64 String2UInt64(const char* str)
{
	u64 a = 0;
	sscanf(str, "%llu", &a);
	return a;
}
inline f32 String2Float(const char* str)
{
	return (float)::atof(str);
}
inline void StringSplit(const std::string& strs, char flag, std::vector<std::string>& strlist)
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
template<typename T>
void ArrayToString(std::string& out, std::vector<T>& array, char flag = ':')
{
	if (!array.empty())
	{
		std::ostringstream os;
		for (size_t i = 0;i < array.size();++i)
		{
			os << array[i] << flag;
		}
		out = os.str();
		out.pop_back(); // last flag;
	}
}
}
#endif