#include "../../../include/net/uv/asUvThread.h"
#include <functional>
#include "../../../include/net/uv/asUvNetWork.h"

asUvThread::asUvThread()
	:m_thread(0),m_loop(nullptr),m_isStoped(false)
{
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
	m_loop->data = this;
	uv_async_init(m_loop, &m_async, [](uv_async_t* handle) {
		// 空回调，仅用于唤醒
		});
	auto ThreadFunc = [](void* arg) 
		{
			asUvThread* self = static_cast<asUvThread*>(arg);
			if (!self) return;
			while (!self->m_isStoped)
			{
				std::queue<std::function<void()>> tmp;
				{
					std::lock_guard<std::mutex> lock(self->m_taskMutex);
					std::swap(tmp, self->m_tasks);
				}
				while (!tmp.empty())
				{
					auto& task = tmp.front();
					task();
					tmp.pop();
				}
				uv_run(self->m_loop, UV_RUN_NOWAIT);
			}
			std::queue<std::function<void()>> tmp;
			{
				std::lock_guard<std::mutex> lock(self->m_taskMutex);
				std::swap(tmp, self->m_tasks);
			}
			while (!tmp.empty())
			{
				auto& task = tmp.front();
				task();
				tmp.pop();
			}
			// 清理各种资源
			printf("asUvThread::StopThread, thread id : %lu\n", self->m_thread);
			// 定时器
			if (self->m_timerFunc)
			{
				self->m_timerFunc = nullptr;
				uv_timer_stop(&self->m_timer);
				if (!uv_is_closing((uv_handle_t*)&self->m_timer))
				{
					uv_close((uv_handle_t*)&self->m_timer, nullptr);
				}
			}
			// 异步事件
			if (!uv_is_closing((uv_handle_t*)&self->m_async)) {
				uv_close((uv_handle_t*)&self->m_async, nullptr);
			}
			// session
			for (auto itr : self->m_sessions)
			{
				asUvSession* session = itr.second;
				if (session)
				{
					uv_read_stop((uv_stream_t*)&session->m_socket);
					uv_close((uv_handle_t*)&session->m_socket, [](uv_handle_t* handle) 
						{
							asUvSession* s = static_cast<asUvSession*>(handle->data);
							if (s)
							{
								delete s;
								s = nullptr;
							}
						});
				}
			}
			self->m_sessions.clear();
			
			while(uv_run(self->m_loop,UV_RUN_NOWAIT) != 0)
			{
				// 空转
			}
			if (self->m_loop)
			{
				//uv_stop(self->m_loop);
				uv_loop_close(self->m_loop);
				delete self->m_loop;
				self->m_loop = nullptr;
			}
		};
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
	m_isStoped = true;
	uv_thread_join(&m_thread);
	delete this;
	return true;
}

void asUvThread::ClearAllSession()
{
	for (auto itr : m_sessions)
	{
		asUvSession* session = itr.second;
		if (session)
		{
			delete session;
			session = nullptr;
		}
	}
	m_sessions.clear();
}

void asUvThread::PostEvent(std::function<void()> event)
{
	{
		std::lock_guard<std::mutex> lock(m_taskMutex);
		m_tasks.push(std::move(event));
	}
	uv_async_send(&m_async);
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
		if (!uv_is_closing((uv_handle_t*)&m_timer))
		{
			uv_timer_stop(&m_timer);
			uv_close((uv_handle_t*)&m_timer, nullptr);
		}
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
