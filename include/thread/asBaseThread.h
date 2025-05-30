﻿#ifndef AS_BASETHREAD_H
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

	void StopThread();

	bool IsRunning();

	asThreadHandle GetHandle();

	void SetThreadName(const char* name,const wchar_t* wname);

	static void Sleep(u32 ms);

	static void ThreadYield();

	static asThreadHandle GetCurrentHandle();

	static void SetCurrentThreadName(const char* name,const wchar_t* wname);

protected:
	std::thread* m_thread;
	asThreadFunc* m_func;
private:
	u32 m_threadId;
};

#endif
