#ifndef AS_UVTHREAD_H
#define AS_UVTHREAD_H

#include "asUvSession.h"
#include <map>
#include <queue>
#include <functional>
#include <mutex>


// 一个thread维护一个loop
class asUvNetWork;

class asUvThread
{
	friend class asUvNetWork;
public:
	asUvThread();
	~asUvThread();

	bool InitThread();

	bool StopThread();

	void PostEvent(std::function<void()> event);

	void StartTimer(std::function<void()> callback, u64 timeout_ms, u64 repeat_ms = 0);

	void StopTimer();

	asUvSession* GetSession(u32 sessionId);

public:
	uv_thread_t						m_thread;
	uv_loop_t*						m_loop;
	uv_async_t						m_async;
	uv_timer_t						m_timer; // 单任务定时器
	std::function<void()>			m_timerFunc; // 任务函数
private:
	std::map<u32, asUvSession*>			m_sessions;	
	std::queue<std::function<void()>>	m_tasks;
	std::mutex							m_taskMutex;
	bool								m_isStoped;		
};
#endif // !AS_UVTHREAD_H

