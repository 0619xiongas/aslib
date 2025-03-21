#include "asPath.h"

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include 
#endif

std::string asPath::RemoveFileName(const char* path)
{
	std::string ret;
	std::string sPath(path);
	for (size_t i = sPath.size() - 1; i > 0; --i)
	{
		if (sPath[i] == '/' || sPath[i] == '\\')
		{
			ret = sPath.substr(0, i + 1);
			break;
		}
	}
	return ret;
}

std::string asPath::RemoveFileExtend(const char* path)
{
	std::string ret;
	std::string sPath(path);
	for (size_t i = sPath.size() - 1; i > 0; --i)
	{
		if (sPath[i] == '.')
		{
			ret = sPath.substr(0, i + 1);
			break;
		}
	}
	return ret;
}

std::string asPath::GetFileExtend(const char* path)
{
	std::string ret;
	std::string sPath(path);
	for (size_t i = sPath.size() - 1; i > 0; --i)
	{
		if (sPath[i] == '.')
		{
			ret = sPath.substr(i, sPath.size() - 1);
			break;
		}
	}
	return ret;
}

std::string asPath::GetFileBaseName(const char* path, bool ext)
{
	std::string ret;
	std::string sPath(path);
	size_t extIndex = 0;
	for (size_t i = sPath.size() - 1; i > 0; --i)
	{
		if (sPath[i] == '.')
		{
			extIndex = i;
		}
		if (sPath[i] == '/' || sPath[i] == '\\')
		{
			ret = sPath.substr(i + 1, sPath.size() - 1);
			break;
		}
	}
	if (!ext && ret.size() > 0)
	{
		for (size_t i = ret.size() - 1; i > 0; --i)
		{
			if (ret[i] == '.')
			{
				ret = ret.substr(0, i);
				break;
			}
		}
	}
	return ret;
}

bool asPath::IsDirExist(const char* path)
{
#ifdef _WIN32
	return ::_access(path, 0) != -1;
#else
	return ::access(path, 0) != -1;
#endif
}

bool asPath::MakeDir(const char* path)
{
	std::string sPath(path);
	if (sPath.size() > 1024)
	{
		return false;
	}
	if (sPath[sPath.size() - 1] != '/' || sPath[sPath.size() - 1] != '\\')
	{
		sPath += '/';
	}
	char tmp[1024] = { 0 };
	for (size_t i = 0; i < 1024 && i < sPath.size() - 1; ++i)
	{
		tmp[i] = sPath[i];
		if (tmp[i] == '/' || tmp[i] == '\\')
		{
#ifdef _WIN32
			if (::_access(tmp, 0) == -1)
			{
				int ret = ::_mkdir(tmp);
#else		
			if (::access(tmp, 0) == -1)
			{
				int ret = ::mkdir(tmp, S_IRWXU);
#endif		
				if (ret == -1)
					return false;
			}
		}
	}
	return true;
}
