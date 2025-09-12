#include "asAsioThread.h"
#include <functional>
asAsioThread::asAsioThread():
	m_work(new asio::io_context::work(m_ioc)),m_thread(std::bind(&asAsioThread::ThreadFunc,this)),
	m_sockTmp(nullptr)
{
	
}

asAsioThread::~asAsioThread()
{
	m_work.reset();
	ClearSession();
	AS_SAFE_DELETE(m_sockTmp);
}

void asAsioThread::ClearSession()
{
	for (auto itr = m_sessions.begin();itr != m_sessions.end(); ++itr)
	{
		asio::error_code ec1, ec2;
		itr->second->GetSocket()->shutdown(asio::ip::tcp::socket::shutdown_both, ec1);
		itr->second->GetSocket()->close(ec2);
		m_asioSeesionPool.Destroy(itr->second);
	}
	m_sessions.clear();
}

u32 asAsioThread::GetSessionCount()
{
	return (u32)m_sessions.size();
}

asAsioSession* asAsioThread::GetSession(u32 id)
{
	asAsioSession* ptr = nullptr;
	auto itr = m_sessions.find(id);
	if (itr != m_sessions.end())
	{
		ptr = itr->second;
	}
	return ptr;
}

void asAsioThread::ThreadFunc()
{
	m_ioc.run();
}

void asAsioThread::SetThreadName(u32 id, const char* name, const wchar_t* wName)
{
	std::string s(name);
	s += '_';
	s += std::to_string(id);
	std::wstring ws(wName);
	ws += '_';
	ws += std::to_wstring(id);
	std::shared_ptr<asio::steady_timer> timer(new asio::steady_timer(m_ioc, std::chrono::seconds(2)));
	timer->async_wait(std::bind(&asAsioThread::HandleSetThreadName, this, std::placeholders::_1, timer, s, ws));
}

void asAsioThread::HandleSetThreadName(const asio::error_code& ec, std::shared_ptr<asio::steady_timer>& timer,
	const std::string& name, const std::wstring& wName)
{
	if (!ec)
	{
#ifdef _WIN32
		SetThreadDescription(m_thread.native_handle(), wName.c_str());
#else
		pthread_setname_np(m_thread.native_handle(), name.c_str());
#endif
	}
}
