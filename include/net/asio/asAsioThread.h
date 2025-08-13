#ifndef AS_ASIOTHREAD_H
#define AS_ASIOTHREAD_H

#include "asAsioSession.h"
#include <boost/thread.hpp>
#include <map>
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

private:
	boost::asio::io_context				m_ioc;
	std::unique_ptr<boost::asio::io_context::work>			m_work;
	boost::thread					m_thread;
	std::map<u32, asAsioSession*>	m_sessions;
	boost::asio::ip::tcp::socket*	m_sockTmp;
};
#endif // !AS_ASIOTHREAD_H

