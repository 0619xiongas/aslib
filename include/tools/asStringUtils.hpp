#ifndef AS_STRINGUTILS_H
#define AS_STRINGUTILS_H
#include "../asBaseDefine.h"
#include <cstdlib>
#include <string>
#include <sstream>
#include <vector>


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

std::string Int32ToString(i32 a);
std::string Int64ToString(i64 a);
std::string floatToStr(f32 a);
i32 String2Int32(const char* str);
u32 String2UInt32(const char* str);
u32 HexString2UInt32(const char* str);
i64 String2Int64(const char* str);
u64 String2UInt64(const char* str);
f32 String2Float(const char* str);
void StringSplit(const std::string& strs, char flag, std::vector<std::string>& strlist);
void StringSplit(const std::string& strs, const char* flag, std::vector<std::string>& strlist);
void StringSplit(const std::string& strs, const char flag1, const char flag2, std::vector<std::vector<std::string>>& lists);

#endif