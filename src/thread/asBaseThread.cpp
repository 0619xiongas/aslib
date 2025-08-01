#include "../../include/thread/asBaseThread.h"
#include <chrono>
#include <functional>
asBaseThread::asBaseThread()
	:m_thread(nullptr),m_threadId(0),m_exit(false)
{
}

asBaseThread::~asBaseThread()
{
	StopThread();
}

bool asBaseThread::StartThread(asThreadFunc* func)
{
	if (func)
	{
		m_thread = new std::thread(*func);
		return true;
	}
	return false;
}

bool asBaseThread::StartThread()
{
	if (m_thread) return false;
	m_thread = new std::thread(std::bind(&asBaseThread::ThreadFunc, this));
	if (m_thread)
	{
		return true;
	}
	return false;
}
void asBaseThread::Exit()
{
	m_exit = true;
}


void asBaseThread::StopThread()
{
	if (m_thread)
	{
		m_thread->join();
		delete m_thread;
		m_thread = nullptr;
	}
}

bool asBaseThread::IsRunning()
{
	if (m_thread == nullptr) return false;
	return m_thread->joinable();
}

asThreadHandle asBaseThread::GetHandle()
{
	if (m_thread)
	{
		return m_thread->native_handle();
	}
	return 0;
}

void asBaseThread::SetThreadName(const char* name, const wchar_t* wname)
{
	asThreadHandle handle = GetHandle();
	if (!handle)
		return;
#ifdef _WIN32
	SetThreadDescription(handle, wname);
#else
	pthread_setname_np(handle, name);
#endif // _WIN32

}

void asBaseThread::Sleep(u32 ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void asBaseThread::ThreadYield()
{
	std::this_thread::yield();
}

asThreadHandle asBaseThread::GetCurrentHandle()
{
#ifdef _WIN32
	return GetCurrentThread();
#else
	return pthread_self();
#endif
}

void asBaseThread::SetCurrentThreadName(const char* name, const wchar_t* wname)
{
	asThreadHandle handle = asBaseThread::GetCurrentHandle();
#ifdef _WIN32
	SetThreadDescription(handle, wname);
#else 
	pthread_setname_np(handle, name);
#endif
}

bool asBaseThread::IsNeedExit() const
{
	return m_exit;
}
