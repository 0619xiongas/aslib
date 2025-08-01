#ifndef AS_BASETHREAD_H
#define AS_BASETHREAD_H

#include <thread>
#ifdef _WIN32
#include <windows.h>
typedef HANDLE	asThreadHandle;
#else
#include <pthread.h>
typedef pthread_t	asThreadHandle;
#endif
#include "../asBaseDefine.h"
typedef void(*asThreadFunc)();

class asBaseThread
{
public:
	asBaseThread();
	virtual ~asBaseThread();

	bool StartThread(asThreadFunc* func);

	bool StartThread();

	void Exit();

	void StopThread();

	bool IsRunning();

	asThreadHandle GetHandle();

	void SetThreadName(const char* name, const wchar_t* wname);

	static void Sleep(u32 ms);

	static void ThreadYield();

	static asThreadHandle GetCurrentHandle();

	static void SetCurrentThreadName(const char* name, const wchar_t* wname);

protected:
	virtual void ThreadFunc() = 0;
	bool IsNeedExit() const;
protected:
	std::thread* m_thread;
	bool m_exit;
public:
	u32 m_threadId;
};

#endif
