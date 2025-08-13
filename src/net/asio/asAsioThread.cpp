#include "../../../include/net/asio/asAsioThread.h"

asAsioThread::asAsioThread():
	m_work(std::make_unique<boost::asio::io_context::work>(m_ioc)),m_thread(boost::bind(&boost::asio::io_context::run,&m_ioc)),
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
		boost::system::error_code ec1, ec2;
		itr->second->GetSocket()->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec1);
		itr->second->GetSocket()->close(ec2);
		delete itr->second;
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