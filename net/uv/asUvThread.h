#ifndef AS_UVTHREAD_H
#define AS_UVTHREAD_H

#include "asUvSession.h"
#include <map>
#include <functional>
#include <stdarg.h>


// һ��threadά��һ��loop
class asUvNetWork;

class asUvThread
{
	friend class asUvNetWork;
public:
	asUvThread();
	~asUvThread();

	bool InitThread();

	void ClearAllSession();

	void PostEvent(std::function<void()> event);

	void StartTimer(std::function<void()> callback, u64 timeout_ms, u64 repeat_ms = 0);

	void StopTimer();

	asUvSession* GetSession(u32 sessionId);

public:
	uv_thread_t						m_thread;
	uv_loop_t*						m_loop;
	uv_async_t						m_async; //�첽������
	uv_timer_t						m_timer; // ������ʱ��
	std::function<void()>			m_timerFunc; // ������
	uv_idle_t						m_idle;
private:
	std::map<u32, asUvSession*>		m_sessions;						
};
#endif // !AS_UVTHREAD_H

