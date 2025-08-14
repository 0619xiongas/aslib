#include "../../../include/net/asio/asAsioNetwork.h"
#include "../../../include/log/asLogger.h"
asAsioNetwork::asAsioNetwork()
	: m_acceptor(nullptr), m_threads(nullptr), m_threadNum(1), m_maxSessionCount(128),
	m_recvBufSize(8 * 1024), m_sendBufSize(8 * 1024), m_isClient(false),
	m_sessionIDAlloc(0),m_isStoped(false),m_name("")
{
}

asAsioNetwork::~asAsioNetwork()
{
	TryStopNetwork();
}

void asAsioNetwork::Init(const char* ip, i32 port, u32 sendBufSize, u32 recvBufSize, u32 threadCount, u32 sessionCount, std::string netName)
{
	boost::asio::ip::address addr;
	addr = addr.from_string(ip);
	m_addr.address(addr);
	m_addr.port(port);
	m_sendBufSize = sendBufSize;
	m_recvBufSize = recvBufSize;
	m_maxSessionCount = sessionCount;
	m_threadNum = threadCount;
	m_name = netName;
}

bool asAsioNetwork::TryRunNetwork(bool isClient)
{
	if(m_threads == nullptr && !m_isStoped)
	{
		m_isClient = isClient;
		m_threads = new asAsioThread* [m_threadNum];
		for(u32 i = 0;i < m_threadNum;++i)
		{
			m_threads[i] = new asAsioThread();
		}
		if(m_isClient)
		{
			ConnectToServer();
		}
		else
		{
			bool isv4 = m_addr.address().is_v4();
			try
			{
				m_acceptor = new boost::asio::ip::tcp::acceptor(m_threads[0]->m_ioc);
				m_acceptor->open(isv4 ? boost::asio::ip::tcp::v4() : boost::asio::ip::tcp::v6());
				m_acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
				m_acceptor->bind(m_addr);
				m_acceptor->listen();
			}
			catch (boost::system::system_error& ec)
			{
				AS_LOGGER->LogEx(LOGTYPE::ERR, "%s start listen error, %s", m_name.c_str(), ec.what());
				return false;
			}
			catch (...)
			{
				AS_LOGGER->Log(LOGTYPE::ERR, "asAsioNetwork TryRunNetwork error");
				return false;
			}
			DoAccept();
			AS_LOGGER->LogEx(LOGTYPE::TIP, "%s start listen %s : %d ...", m_name.c_str(), m_addr.address().to_string().c_str(), m_addr.port());
		}
		if (m_isStoped)
		{
			return false;
		}
	}
	return true;
}

bool asAsioNetwork::TryStopNetwork()
{
	if (m_isStoped)
		return false;
	AS_LOGGER->LogEx(TIP, "%s TryStopNetwork ...", m_name.c_str());
	m_isStoped = true;
	if (!m_isClient && m_acceptor)
	{
		m_acceptor->cancel();
		m_acceptor->close();
		delete m_acceptor;
	}
	if (m_threads)
	{
		for (u32 i = 0; i < m_threadNum; ++i)
		{
			if (m_threads[i])
			{
				m_threads[i]->m_ioc.stop();
				m_threads[i]->m_thread.join();
				m_threads[i]->ClearSession();
				AS_SAFE_DELETE(m_threads[i]);
			}
		}
		AS_SAFE_DELETE_ARRAY(m_threads)
	}
	AS_LOGGER->LogEx(TIP, "%s Stopped Network ...", m_name.c_str());
	return false;
}

u32 asAsioNetwork::ConnectToServer()
{
	if (this->GetSessionCount() >= m_maxSessionCount)
	{
		return 0;
	}
	if (m_threads == nullptr)
	{
		m_threads = new asAsioThread* [m_threadNum];
		for (u32 i = 0; i < m_threadNum; ++i)
		{
			m_threads[i] = new asAsioThread();
		}
	}
	u32 id = ++m_sessionIDAlloc;
	// 默认客户端模式只开一条线程 threadID = 0
	boost::system::error_code ec;
	boost::asio::ip::tcp::socket* sock = new boost::asio::ip::tcp::socket(m_threads[0]->m_ioc);
	m_threads[0]->m_sockTmp = sock;
	do 
	{
		AS_LOGGER->LogEx(TIP, "%s connect to %s : %d ...", m_name.c_str(), m_addr.address().to_string().c_str(), m_addr.port());
		sock->connect(m_addr, ec);
		if (!ec)
		{
			asAsioSession* sessionPtr = new asAsioSession();
			sessionPtr->Init(id, sock, m_recvBufSize, m_sendBufSize, this);
			m_threads[0]->m_sessions.insert(std::pair<u32, asAsioSession*>(id, sessionPtr));
			StartReceive(sessionPtr);
			OnAddNewSession(*sessionPtr);
			m_threads[0]->m_sockTmp = nullptr;
			return id;
		}
		asBaseThread::Sleep((u32)5 * 1000);
	} while (ec && !m_isStoped);
	AS_LOGGER->LogEx(TIP, "%s connect to %s : %d success !!!", m_name.c_str(), m_addr.address().to_string().c_str(), m_addr.port());
	return 0;
}

void asAsioNetwork::DoCloseSession(u32 sessionId)
{
	u32 threadID = sessionId % m_threadNum;
	m_threads[threadID]->m_ioc.post(std::bind(&asAsioNetwork::HandleCloseSession, this, threadID, sessionId));
}

u32 asAsioNetwork::GetSessionCount()
{
	u32 ret = 0;
	for (u32 i = 0; i < m_threadNum; ++i)
	{
		if (m_threads&&m_threads[i])
		{
			ret += (u32)m_threads[i]->GetSessionCount();
		}
	}
	return ret;
}

void asAsioNetwork::DoSendData(char* buf, u32 len, u32 sessionId)
{
	u32 threadID = sessionId % m_threadNum;
	if (m_threads && m_threads[threadID])
	{
		AS_SHARED_ARRAY<char> sbuf = make_shared_array<char>(buf);
		m_threads[threadID]->m_ioc.post(boost::bind(&asAsioNetwork::HandleSendData, this, threadID, sessionId, sbuf, len));
	}
	else
	{
		AS_SAFE_DELETE_ARRAY(buf);
	}
}

void asAsioNetwork::DoSendDataAll(char* buf, u32 len)
{
	if (m_threads)
	{
		AS_SHARED_ARRAY<char> sbuf = make_shared_array<char>(buf);
		for (u32 i = 0; i < m_threadNum; ++i)
		{
			if (m_threads[i])
			{
				m_threads[i]->m_ioc.post(boost::bind(&asAsioNetwork::HandleSendDataAll, this, i, sbuf, len));
			}
		}
	}
	else
	{
		AS_SAFE_DELETE_ARRAY(buf);
	}
}

void asAsioNetwork::DoSendDataSome(char* buf, u32 len, std::vector<u32>& sessions)
{
	if (m_threads)
	{
		AS_SHARED_ARRAY<char> sbuf = make_shared_array<char>(buf);
		for (size_t i = 0; i < sessions.size(); ++i)
		{
			u32 threadID = sessions[i] % m_threadNum;
			if (m_threads[threadID])
			{
				m_threads[threadID]->m_ioc.post(boost::bind(&asAsioNetwork::HandleSendData, this, threadID, sessions[i], sbuf, len));
			}
		}
	}
	else
	{
		AS_SAFE_DELETE_ARRAY(buf);
	}
}

void asAsioNetwork::OnNewMessage(asAsioSession& session, u32 msgId, char* buf)
{
	AS_SAFE_DELETE_ARRAY(buf);
}

void asAsioNetwork::SetNetworkName(const char* name, const wchar_t* wName)
{
	if (!m_threads)
		return;
	for (u32 i = 0; i < m_threadNum; ++i)
	{
		if (m_threads[i])
		{
			m_threads[i]->SetThreadName(i,name, wName);
		}
	}
}

void asAsioNetwork::DoAccept()
{
	u32 id = ++m_sessionIDAlloc;
	boost::asio::ip::tcp::socket* sock = new boost::asio::ip::tcp::socket(m_threads[id % m_threadNum]->m_ioc);
	m_threads[id % m_threadNum]->m_sockTmp = sock;
	m_acceptor->async_accept(*sock, boost::bind(&asAsioNetwork::HandleAccept, this, id, sock, boost::placeholders::_1));
}

void asAsioNetwork::HandleAccept(u32 sessionID, boost::asio::ip::tcp::socket* sock, const boost::system::error_code& ec)
{
	if (ec == boost::asio::error::operation_aborted)
	{
		AS_SAFE_DELETE(sock);
		m_threads[sessionID % m_threadNum]->m_sockTmp = nullptr;
		return;
	}
	if (sock)
	{
		if (!ec)
		{
			if (this->GetSessionCount() > m_maxSessionCount)
			{
				boost::system::error_code ec1, ec2;
				sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec1);
				sock->close(ec2);
				delete sock;
				m_threads[sessionID % m_threadNum]->m_sockTmp = nullptr;
			}
			else
			{
				asAsioSession* sessionPtr = new asAsioSession();
				sessionPtr->Init(sessionID, sock, m_recvBufSize, m_sendBufSize, this);
				u32 threadID = sessionID % m_threadNum;
				m_threads[threadID]->m_ioc.post(boost::bind(&asAsioNetwork::HandleAddSession, this, threadID, sessionPtr));
				m_threads[threadID]->m_sockTmp = nullptr;
			}
		}
		else
		{
			boost::system::error_code ec1, ec2;
			sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec1);
			sock->close(ec2);
			delete sock;
			m_threads[sessionID % m_threadNum]->m_sockTmp = nullptr;
			
			AS_LOGGER->LogEx(ERR, "%s HandleAccept seesion %u, occured error %s", m_name.c_str(), ec.message().c_str());
		}
	}
	DoAccept();
}

void asAsioNetwork::HandleAddSession(u32 threadID, asAsioSession* sessionPtr)
{
	m_threads[threadID]->m_sessions.insert(std::pair<u32, asAsioSession*>(sessionPtr->GetID(), sessionPtr));
	StartReceive(sessionPtr);
	OnAddNewSession(*sessionPtr);
}

void asAsioNetwork::StartReceive(asAsioSession* sessionPtr)
{
	sessionPtr->m_socket->async_read_some(boost::asio::buffer(sessionPtr->m_recvBuf.CurrentBuf(), sessionPtr->m_recvBuf.RemainSize()),
		boost::bind(&asAsioNetwork::HandleReceived,this,boost::placeholders::_1,boost::placeholders::_2,sessionPtr));
}

void asAsioNetwork::HandleReceived(const boost::system::error_code& ec, size_t bytestransfered, asAsioSession* sessionPtr)
{
	if (!sessionPtr)
	{
		AS_LOGGER->LogEx(ERR, "%s HandleReceived, asAsioSession ptr is nullptr !!! ",m_name.c_str());
		return;
	}
	u32 sessionID = sessionPtr->GetID();
	if (ec)
	{
		HandleCloseSession(sessionID % m_threadNum, sessionID);
		return;
	}
	sessionPtr->m_recvBuf.AddSize(bytestransfered);
	sessionPtr->OnReceiveData();
	StartReceive(sessionPtr);
}

void asAsioNetwork::HandleCloseSession(u32 threadID, u32 sessionID)
{
	asAsioThread* th = m_threads[threadID];
	asAsioSession* sessionPtr = th->GetSession(sessionID);
	if (sessionPtr)
	{
		try
		{
			boost::system::error_code ec1, ec2;
			sessionPtr->m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec1);
			sessionPtr->m_socket->close(ec2);
		}
		catch (boost::system::system_error& e)
		{
			AS_LOGGER->LogEx(ERR, "%s HandleCloseSession %u, occured error, %s", m_name.c_str(), sessionID, e.what());
		}
		catch (boost::system::error_code& e)
		{
			AS_LOGGER->LogEx(ERR, "%s HandleCloseSession %u, occured error, %s", m_name.c_str(), sessionID, e.message().c_str());
		}
		th->m_sessions.erase(sessionID);
		delete sessionPtr;
		sessionPtr = nullptr;
		OnCloseSession(sessionID);
	}
}

void asAsioNetwork::HandleSendData(u32 threadID, u32 sessionID, AS_SHARED_ARRAY<char>buf, u32 len)
{
	if (buf && len > 0)
	{
		asAsioSession* sessionPtr = m_threads[threadID]->GetSession(sessionID);
		if(sessionPtr)
		{
			WriteData(m_threads[threadID], sessionPtr, buf, len);
		}
	}
}

void asAsioNetwork::HandleSendDataAll(u32 threadID, AS_SHARED_ARRAY<char>buf, u32 len)
{
	if (buf && len > 0)
	{
		if (m_threads[threadID])
		{
			std::map<u32, asAsioSession*>& sessions = m_threads[threadID]->m_sessions;
			for (auto itr = sessions.begin(); itr != sessions.end(); ++itr)
			{
				WriteData(m_threads[threadID], itr->second, buf, len);
			}
		}
	}
}

void asAsioNetwork::WriteData(asAsioThread* pThread, asAsioSession* sessionPtr, AS_SHARED_ARRAY<char> buf, u32 len)
{
	if (sessionPtr->m_pending)
	{
		sessionPtr->m_sendQueue.Push(buf, len);
	}
	else if (sessionPtr->m_sendQueue.m_size > 0)
	{
		sessionPtr->m_sendQueue.Push(buf, len);
		SendSessionQueue(pThread, sessionPtr);
	}
	else
	{
		sessionPtr->m_pending = true;
		boost::asio::async_write(*sessionPtr->m_socket, boost::asio::buffer(buf.get(),len),
			boost::bind(&asAsioNetwork::HandleWrited,this,boost::placeholders::_1,boost::placeholders::_2,sessionPtr,buf));
	}
}

void asAsioNetwork::SendSessionQueue(asAsioThread* pThread, asAsioSession* sessionPtr)
{
	AS_SHARED_ARRAY<char> buf = make_shared_array(new char[sessionPtr->m_sendQueue.m_size]);
	asBuffer wb; wb.Init(buf.get(), sessionPtr->m_sendQueue.m_size);
	sessionPtr->m_pending = true;
	sessionPtr->m_sendQueue.MergeQueue2Buf(wb);
	boost::asio::async_write(*sessionPtr->m_socket, boost::asio::buffer(wb.Buf(), wb.Size()),
		boost::bind(&asAsioNetwork::HandleWrited, this, boost::placeholders::_1, boost::placeholders::_2, sessionPtr, buf));
}

void asAsioNetwork::HandleWrited(const boost::system::error_code& ec, size_t bytestransfered, asAsioSession* sessionPtr, AS_SHARED_ARRAY<char> buf)
{
	u32 sessionID = sessionPtr->GetID();
	if (ec)
	{
		HandleCloseSession(sessionID % m_threadNum, sessionID);
	}
	else
	{
		if (sessionPtr)
		{
			sessionPtr->m_pending = false;
			if (sessionPtr->m_sendQueue.m_size > 0)
			{
				SendSessionQueue(m_threads[sessionID % m_threadNum], sessionPtr);
			}
		}
	}
}

void asAsioNetwork::OnAddNewSession(asAsioSession& session)
{
}

void asAsioNetwork::OnCloseSession(u32 sessionId)
{

}