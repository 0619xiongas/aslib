#include "asProcess.h"
#if _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <string.h>
#endif
#include "../file/asPath.h"
asProcess::asProcess()
	:m_pid(0)
{
	GetInfo();
}

asProcess::~asProcess()
{
}

void asProcess::GetInfo()
{
	char fullPath[1024] = { 0 };
#ifdef _WIN32
	::GetModuleFileNameA(NULL, static_cast<LPSTR>(fullPath), 1024);
	m_pid = ::_getpid();
#else
	readlink("/proc/self/exe", fullPath, 1024);
	m_id = ::getpid();
#endif
	m_fullPath = fullPath;
	m_path = asPath::RemoveFileName(fullPath);
	m_name = asPath::GetFileBaseName(fullPath, true);
}

std::string asProcess::GetNameNoExtend()
{
	return asPath::RemoveFileExtend(m_name.c_str());
}
