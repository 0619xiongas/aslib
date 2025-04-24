#include "asUvThread.h"
#include <functional>
#include "asUvNetWork.h"

asUvThread::asUvThread()
	:m_thread(0)
{
	m_async.data = nullptr;
}

asUvThread::~asUvThread()
{
	StopTimer();
	if (m_loop)
	{
		uv_loop_close(m_loop);
		delete m_loop;
	}
}

bool asUvThread::InitThread()
{
	m_loop = new uv_loop_t;
	if (uv_loop_init(m_loop) != 0)
	{
		printf("Init loop fail");
		return false;
	}
	auto ThreadFunc = [](void* arg) {
		asUvThread* self = static_cast<asUvThread*>(arg);
		uv_run(self->m_loop, UV_RUN_DEFAULT);
		};
	auto AsyncFunc = [](uv_async_t* handle) {
		auto* event = static_cast<std::function<void()>*>(handle->data);
		(*event)();
		delete event;
		};
	auto IdleFunc = [](uv_idle_t* handle) {
		auto self = static_cast<asUvThread*>(handle->data);
		if (self)
		{

		}
		else
		{
			uv_stop(handle->loop);
		}
		};
	uv_async_init(m_loop, &m_async, AsyncFunc);
	uv_idle_init(m_loop, &m_idle);
	m_idle.data = this;
	uv_idle_start(&m_idle, IdleFunc);
	i32 ret = uv_thread_create(&m_thread,ThreadFunc, this);
	if (ret != 0)
	{
		printf("Init Uv Thread fail, %s", uv_strerror(ret));
		return false;
	}
	return true;
}

void asUvThread::ClearAllSession()
{
	for (auto itr: m_sessions)
	{
		uv_close((uv_handle_t*)&(itr.second->m_socket), [](uv_handle_t* handle) {
			asUvSession* session = static_cast<asUvSession*>(handle->data);
			if (session)
			{
				delete session;
				session = nullptr;
			}
			});
	}
	m_sessions.clear();
}

void asUvThread::PostEvent(std::function<void()> event)
{
	auto* task = new std::function<void()>(std::move(event));
	m_async.data = task;
	if (uv_async_send(&m_async) != 0)
	{
		delete task;
		m_async.data = nullptr;
	}
}

void asUvThread::StartTimer(std::function<void()> callback, u64 timeout_ms, u64 repeat_ms)
{
	auto TimerFunc = [](uv_timer_t* handle)
		{
			asUvThread* self = static_cast<asUvThread*>(handle->data);
			if (self && self->m_timerFunc)
			{
				self->m_timerFunc();
			}
		};
	PostEvent([this, callback, timeout_ms, repeat_ms, TimerFunc]() {
		m_timerFunc = callback;
		m_timer.data = this;
		uv_timer_init(m_loop, &m_timer);
		uv_timer_start(&m_timer, TimerFunc, timeout_ms, repeat_ms);
		});
}

void asUvThread::StopTimer()
{
	PostEvent([this]() {
		uv_timer_stop(&m_timer);
		uv_close((uv_handle_t*)&m_timer, nullptr);
		});
}

asUvSession* asUvThread::GetSession(u32 sessionId)
{
	auto itr = m_sessions.find(sessionId);
	if (itr != m_sessions.end())
	{
		return itr->second;
	}
	return nullptr;
}
