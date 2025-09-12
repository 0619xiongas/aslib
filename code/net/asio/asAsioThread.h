#ifndef AS_ASIOTHREAD_H
#define AS_ASIOTHREAD_H

#include "asAsioSession.h"
#include <map>
#include <thread>

#include "../../memory/asMemoryPool.h"
class asAsioNetwork;
class asAsioThread
{
	friend class asAsioNetwork;
public:
	asAsioThread();
	~asAsioThread();

	void ClearSession();

	u32 GetSessionCount();

	asAsioSession* GetSession(u32 id);

	void ThreadFunc();

	void SetThreadName(u32 id, const char* name, const wchar_t* wName);
private:
	void HandleSetThreadName(const asio::error_code& ec, std::shared_ptr<asio::steady_timer>& timer,
		const std::string& name, const std::wstring& wName);
private:
	asio::io_context				m_ioc;
	std::unique_ptr<asio::io_context::work>			m_work;
	std::thread					m_thread;
	std::map<u32, asAsioSession*>	m_sessions;
	asio::ip::tcp::socket*	m_sockTmp;
	asMemoryPool<asAsioSession,1000>		m_asioSeesionPool;
};
#endif // !AS_ASIOTHREAD_H

