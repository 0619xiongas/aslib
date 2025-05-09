#ifndef AS_DBDATAUTILS_H
#define AS_DBDATAUTILS_H

#include <map>
#include <set>
#include "asStringUtils.hpp"
using namespace astronaut;
class asDBDataUtils
{
public:
	template<typename T>
	static std::string ToDBString(const T& v)
	{
		return v.SerializeDBString(); // DBData类需要实现该方法
	}
	static std::string ToDBString(i64 v)
	{
		return Int64ToString(v);
	}
	static std::string ToDBString(i32 v)
	{
		return Int32ToString(v);
	}
	static const std::string& ToDBString(const std::string& v)
	{
		return v;
	}
	template <typename T>
	static void DBStringTo(std::string& db, T* v)
	{
		v.ParseDBString(db);
	}
	static void DBStringTo(std::string& db, i64* v)
	{
		*v = String2Int64(db.c_str());
	}
	static void DBStringTo(std::string& db, i32* v)
	{
		*v = String2Int32(db.c_str());
	}
	static void DBStringTo(std::string& db, std::string* v)
	{
		*v = db;
	}
public:
	// string data
	template<typename T>
	static void ParseDBString(const std::string& str, std::vector<T>& out)
	{
		if (str.empty())
		{
			return;
		}
		std::vector<std::string> vec;
		StringSplit(str, ',', vec);
		out.resize(vec.size());
		for (size_t i = 0;i < vec.size();++i)
		{
			DBStringTo(vec[i], &out[i]);
		}
	}
	template<typename T>
	static std::string SerializeDBString(const std::vector<T>& vec)
	{
		std::string res;
		for (size_t i = 0;i < vec.size();++i)
		{
			if (i != 0)
			{
				res += ",";
			}
			res += ToDBString(vec[i]);
		}
		return res;
	}
	template<typename T>
	static void ParseDBString(const std::string& str, std::set<T>& out)
	{
		if (str.empty())
		{
			return;
		}
		T t;
		std::vector<std::string> vec;
		StringSplit(str, ',', vec);
		for (size_t i = 0;i < vec.size();++i)
		{
			DBStringTo(vec[i], &t);
			out.insert(t);
		}
	}
	template<typename T>
	static std::string SerializeDBString(const std::set<T>& s)
	{
		std::string res;
		for (auto it = s.begin();it != s.end();++it)
		{
			if (it != s.begin())
			{
				res += ",";
			}
			res += ToDBString(*it);
		}
		return res;
	}
	template<typename T1, typename T2>
	static void ParseDBString(const std::string& str, std::vector<std::pair<T1, T2>>& out)
	{
		if (str.empty())
		{
			return;
		}
		std::pair<T1, T2> p;
		std::vector<std::string> vec;
		StringSplit(str, ',', vec);
		out.reserve(vec.size());
		for (size_t i = 0;i < vec.size();++i)
		{
			std::vector<std::string> subVec;
			StringSplit(str, ':', subVec);
			if (subVec.size() != 2)
			{
				continue;
			}
			DBStringTo(subVec[0], &p.first());
			DBStringTo(subVec[1], &p.second());
			out.push_back(p);
		}
	}
	template<typename T1, typename T2>
	static std::string SerializeDBString(const std::vector<std::pair<T1, T2>>& vec)
	{
		std::string res;
		for (size_t i = 0;i < vec.size();++i)
		{
			if (i != 0)
			{
				res += ",";
			}
			res += ToDBString(vec[i].first);
			res += ":";
			res += ToDBString(vec[i].second);
		}
		return res;
	}
	template<typename T1, typename T2>
	static void ParseDBString(const std::string& str, std::map<T1, T2>& out)
	{
		if (str.empty())
		{
			return;
		}
		T1 t;
		std::vector<std::string> vec;
		StringSplit(str, ',', vec);
		for (size_t i = 0;i < vec.size();++i)
		{
			std::vector<std::string> subVec;
			StringSplit(str, ':', subVec);
			if (subVec.size() != 2)
			{
				continue;
			}
			DBStringTo(subVec[0], &t);
			DBStringTo(subVec[1], &out[t]);
		}
	}
	template<typename T1, typename T2>
	static std::string SerializeDBString(const std::map<T1, T2>& mp)
	{
		std::string res;
		for (auto it = mp.begin();it != mp.end();++it)
		{
			if (it != mp.begin())
			{
				res += ",";
			}
			res += ToDBString(it->first);
			res += ":";
			res += ToDBString(it->second);
		}
		return res;
	}
	template<typename T1, typename T2, typename T3>
	static void ParseDBString(const std::string& str, std::map<T1, std::pair<T2, T3>>& out)
	{
		if (str.empty())
		{
			return;
		}
		T1 t;
		std::pair<T2, T3> p;
		std::vector<std::string >vec;
		StringSplit(str, ',', vec);
		for (size_t i = 0;i < vec.size();++i)
		{
			std::vector<std::string >subVec;
			StringSplit(str, ':', subVec);
			if (subVec.size() != 2)
			{
				continue;
			}
			DBStringTo(subVec[0], &t);
			std::string pair = subVec[1];
			StringSplit(pair, '_', subVec);
			if (subVec.size() != 2)
			{
				continue;
			}
			DBStringTo(subVec[0], &p.first);
			DBStringTo(subVec[1], &p.second);
			out[t] = p;
		}
	};
	template<typename T1, typename T2, typename T3>
	static std::string SerializeDBString(const std::map<T1, std::pair<T2, T3>>& mp)
	{
		std::string res;
		for (auto it = mp.begin();it != mp.end();++it)
		{
			if (it != mp.begin())
			{
				res += ",";
			}
			res += ToDBString(it->first);
			res += ":";
			res += ToDBString(it->second.first);
			res += "_";
			res += ToDBString(it->second.second);
		}
		return res;
	}

	// blob data
	template<typename T>
	static void ParseDBBlob(const std::string& blob, std::vector<T>& out)
	{
		
	}

};
#endif // !AS_DBDATAUTILS_H
