#include "../../../include/net/uv/asUvThread.h"
#include <functional>
#include "../../../include/net/uv/asUvNetWork.h"

asUvThread::asUvThread()
	:m_thread(0),m_loop(nullptr),m_isStoped(false)
{
	m_async.data = nullptr;
}

asUvThread::~asUvThread()
{
	StopThread();
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

bool asUvThread::StopThread()
{
	if(m_isStoped) return true;
	std::unique_lock<std::mutex> lock(m_stopMutex);
    m_stopDone = false;
	PostEvent([this](){
		printf("asUvThread::StopThread, thread id : %lu\n", m_thread);
		this->StopTimer();
		this->ClearAllSession();
		if (m_async.data)
		{
			// delete static_cast<std::function<void()>*>(m_async.data);
			// m_async.data = nullptr;
			auto* event = static_cast<std::function<void()>*>(m_async.data);
			(*event)();
			delete event;
			
		}
		uv_close((uv_handle_t*)&m_async, nullptr);

		//停止idle
		uv_idle_stop(&m_idle);
		uv_close((uv_handle_t*)&m_idle, nullptr);
		// 停止loop
		uv_stop(m_loop);
		m_stopDone = true;
		m_stopCv.notify_one();
	});
	uv_thread_join(&m_thread);
	m_stopCv.wait(lock, [this] { 
		if (m_loop)
		{
			uv_loop_close(m_loop);
			delete m_loop;
			m_loop = nullptr;
		}
		return m_stopDone; 
		});
	m_isStoped = true;
	return true;
}

void asUvThread::ClearAllSession()
{
	for (auto itr: m_sessions)
	{
		// 实际析构交给OnClose回调处理
		uv_close((uv_handle_t*)&(itr.second->m_socket), nullptr);
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
