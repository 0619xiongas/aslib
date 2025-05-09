#include "asUvNetWork.h"

asUvNetWork::asUvNetWork()
	:m_addr(),m_socket(), m_sendBufSize(1024 * 10), m_recvBufSize(1024 * 10), m_threadCount(1), m_sessionCount(100),
	m_isClient(false),m_sessionIdAlloc(10000),m_connect()
{
}

asUvNetWork::~asUvNetWork()
{
}

void asUvNetWork::Init(const char* ip, i32 port, u32 sendBufSize, u32 recvBufSize, u32 threadCount, u32 sessionCount)
{
	uv_ip4_addr(ip, port, &m_addr);
	m_sendBufSize = sendBufSize;
	m_recvBufSize = recvBufSize;
	m_threadCount = threadCount;
	m_sessionCount = sessionCount;
}

bool asUvNetWork::TryRunNetWork(bool isClient)
{
	if (m_threads.empty())
	{
		m_isClient = isClient;
		for (u32 i = 0;i < m_threadCount;++i)
		{
			asUvThread* thread = new asUvThread;
			if (thread->InitThread())
			{
				m_threads.push_back(thread);
			}
		}
		if (m_threads.size() != m_threadCount)
		{
			return false;
		}
		if (m_isClient)
		{
			ConnectToServer();
		}
		else
		{
			StartListen();
		}
	}
	return true;
}

bool asUvNetWork::TryStopNetWork()
{
	for (size_t i = 0;i < m_threads.size();++i)
	{
		if (m_threads[i])
		{
			m_threads[i]->PostEvent([this, i]() {
				this->m_threads[i]->StopTimer();
				this->m_threads[i]->ClearAllSession();
				uv_stop(this->m_threads[i]->m_loop);
				});
		}
	}
	for (size_t i = 0;i < m_threads.size();++i)
	{
		if (m_threads[i])
		{
			uv_thread_join(&m_threads[i]->m_thread);
		}
	}
	if (!m_isClient)
	{
		uv_close((uv_handle_t*)&m_socket, nullptr);
	}
	for (size_t i = 0;i < m_threads.size();++i)
	{
		if (m_threads[i])
		{
			delete m_threads[i];
			m_threads[i] = nullptr;
		}
	}
	m_threads.clear();
	return false;
}

u32 asUvNetWork::ConnectToServer()
{
	if (this->GetSessionCount() > 0)
	{
		return 0;
	}
	if (m_threads.empty())
	{
		for (u32 i = 0;i < m_threadCount;++i)
		{
			asUvThread* thread = new asUvThread;
			if (thread->InitThread())
			{
				m_threads.push_back(thread);
			}
		}
		if (m_threads.size() != m_threads.size())
		{
			return 0;
		}
	}
	u32 id = ++m_sessionIdAlloc;
	u32 threadId = id % m_threadCount;
	m_threads[threadId]->PostEvent([this,id,threadId]() {
		this->HandleConnectToServer(id,m_threads[threadId]->m_loop);
		});
	return 0;
}

void asUvNetWork::DoCloseSession(u32 sessionId)
{
	u32 threadId = sessionId % m_threadCount;
	m_threads[threadId]->PostEvent([this,threadId,sessionId](){
		this->HandleCloseSession(threadId, sessionId);
		});
}

u32 asUvNetWork::GetSessionCount()
{
	u32 count = 0;
	for (size_t i = 0;i < m_threadCount;++i)
	{
		count += (u32)m_threads[i]->m_sessions.size();
	}
	return count;
}

void asUvNetWork::DoSendData(char* buf, u32 len, u32 sessionId)
{
	if (!m_threads.empty())
	{
		u32 threadId = sessionId % m_threadCount;
		AS_SHARED_ARRAY<char> sbuf = make_shared_array<char>(buf);
		m_threads[threadId]->PostEvent([this, threadId, sessionId,sbuf, len]() {
			this->HandleSendData(threadId, sessionId, sbuf, len);
			});
	}
	else
	{
		delete[] buf;
	}
}

void asUvNetWork::DoSendDataAll(char* buf, u32 len)
{
	if (!m_threads.empty())
	{
		AS_SHARED_ARRAY<char> data = make_shared_array<char>(buf);
		for (u32 i = 0;i < m_threadCount;++i)
		{
			m_threads[i]->PostEvent([this, i, data, len]() {
				this->HandleSendDataAll(i, data, len);
				});
		}
	}
	else
	{
		delete[] buf;
	}
}

void asUvNetWork::DoSendDataSome(char* buf, u32 len, std::vector<u32>& sessions)
{
	if (!m_threads.empty())
	{
		AS_SHARED_ARRAY<char> sbuf = make_shared_array<char>(buf);
		for (size_t i = 0;i < sessions.size();++i)
		{
			u32 sessionId = sessions[i];
			u32 threadId = sessionId % m_threadCount;
			m_threads[threadId]->PostEvent([this, threadId, sessionId, sbuf, len]() {
				this->HandleSendData(threadId, sessionId, sbuf, len);
				});
		}
	}
}

void asUvNetWork::OnNewMessage(asUvSession& session, u32 msgId, char* buf)
{
}

void asUvNetWork::StartListen()
{
	m_threads[0]->PostEvent([this]() {
		this->HandleListen(m_threads[0]->m_loop);
		});
}

void asUvNetWork::HandleListen(uv_loop_t* loop)
{
	uv_tcp_init(loop, &this->m_socket);
	uv_tcp_bind(&this->m_socket, (const struct sockaddr*)&m_addr, 0);
	// 需要实现一个C风格的uv_connection_cb
	this->m_socket.data = this;
	i32 r = uv_listen((uv_stream_t*)&this->m_socket, 128, [](uv_stream_t* server,int status) {
		auto self = static_cast<asUvNetWork*>(server->data);
		// 具体做
		u32 id = ++self->m_sessionIdAlloc;
		self->DoAccept(id, status);
		});
	if (r) {
		fprintf(stderr, "Listen error %s\n", uv_strerror(r));
		return;
	}
	else
	{
		printf("Server Start Listen : %d", this->m_addr.sin_port);
	}
}

void asUvNetWork::DoAccept(u32 id, int status)
{
	if (status < 0) {
		fprintf(stderr, "New Session error %s\n", uv_strerror(status));
		return;
	}
	u32 offset = id % m_threadCount;
	m_threads[offset]->PostEvent([this, id, offset]() {
		this->HandleAccept(id, m_threads[offset]->m_loop);
		});
}

void asUvNetWork::HandleAccept(u32 id,uv_loop_t* loop)
{
	asUvSession* session = new asUvSession;
	session->Init(id, m_recvBufSize, m_sendBufSize,this);
	uv_tcp_init(loop, &session->m_socket);

	if (uv_accept((uv_stream_t*)&(this->m_socket), (uv_stream_t*)&(session->m_socket)) == 0)
	{
		printf("HandleAccept success session id : %u", id);
		u32 threadId = id % m_threadCount;
		m_threads[threadId]->PostEvent([this,threadId,session ]() {
			this->HandleAddSession(threadId, session);
			});
	}
	else
	{
		session->Close();
	}
}

void asUvNetWork::HandleAddSession(u32 threadId, asUvSession* session)
{
	m_threads[threadId]->m_sessions.insert(std::make_pair(session->GetId(), session));
	StartReceive(session);
	OnAddNewSession(*session);
}

void asUvNetWork::StartReceive(asUvSession* session)
{
	u32 threadId = session->GetId() % m_threadCount;
	m_threads[threadId]->PostEvent([this, session]() {
		uv_read_start((uv_stream_t*)&(session->m_socket), 
			[](uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
			{
				asUvSession* session = static_cast<asUvSession*>(handle->data);
				if (session)
				{
					session->AllocBuf(buf); // 分配空间
				}
			},
			[](uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) 
			{
				asUvSession* session = static_cast<asUvSession*>(stream->data);
				if (session)
				{
					if (nread > 0)
					{
						session->m_recvBuf.AddSize(nread);
						session->OnReceiveData();
					}
					else if(nread < 0)
					{
						session->Close();
					}
				}
			});
		});
}

void asUvNetWork::HandleCloseSession(u32 threadId, u32 sessionId)
{
	asUvThread* pThread = m_threads[threadId];
	asUvSession* session = pThread->GetSession(sessionId);
	if (session && pThread)
	{
		session->OnClose();
		pThread->m_sessions.erase(sessionId);
	}
}

void asUvNetWork::HandleSendData(u32 threadId, u32 sessionId, AS_SHARED_ARRAY<char> buf, u32 len)
{
	if (buf && len)
	{
		asUvThread* pThread = m_threads[threadId];
		asUvSession* pSession = pThread->GetSession(sessionId);
		if (pSession && pThread)
		{
			this->WriteData(pThread, pSession, buf, len);
		}
	}
}

void asUvNetWork::HandleSendDataAll(u32 threadId, AS_SHARED_ARRAY<char> buf, u32 len)
{
	if (buf && len)
	{
		asUvThread* pThread = m_threads[threadId];
		for (auto itr : pThread->m_sessions)
		{
			this->WriteData(pThread, itr.second, buf, len);
		}
	}
}

void asUvNetWork::WriteData(asUvThread* pThread, asUvSession* session, AS_SHARED_ARRAY<char> buf, u32 len)
{
	if (!pThread || !session)
		return;
	// 在发送过程中
	if (session->m_pending)
	{
		session->m_sendQueue.Push(buf, len);
	}
	else/* if (!session->m_sendQueue.m_que.empty())*/ //将数据合并起来发送
	{
		session->m_sendQueue.Push(buf, len);
		session->m_sendQueue.MergeQueue2Buf(session->m_sendBuf);
		session->m_pending = true;

		pThread->PostEvent([this, session]() {
			uv_buf_t data = uv_buf_init(session->m_sendBuf.Buf(), (u32)session->m_sendBuf.Size());
			uv_write(&(session->m_req), (uv_stream_t*)&(session->m_socket), &data, 1, 
				[](uv_write_t* req, int status)
				{
					// 回调中需要处理sendBuf
					asUvSession* session = static_cast<asUvSession*>(req->data);
					if (session)
					{
						session->OnWriteDataCB(status);
					}
				});
			});
	}
}

void asUvNetWork::HandleConnectToServer(u32 sessionId, uv_loop_t* loop)
{
	asUvSession* session = new asUvSession;
	session->Init(sessionId, m_recvBufSize, m_sendBufSize, this);
	uv_tcp_init(loop, &session->m_socket);
	m_connect.data = session;
	TryConnect(session);
}

void asUvNetWork::TryConnect(asUvSession* session)
{
	auto ConnectCB = [](uv_connect_t* req, int status) {
		asUvSession* session = static_cast<asUvSession*>(req->data);
		if (!session) return;
		asUvNetWork* netWork = session->m_netWork;
		if (!netWork) return;
		if (status < 0)
		{
			netWork->ReConnect(session);
		}
		else
		{
			printf("Connect server success!! session id : %u", session->GetId());
			u32 threadId = session->GetId() % session->m_netWork->m_threadCount;
			netWork->m_threads[threadId]->PostEvent([threadId, session,netWork]() {
				netWork->m_threads[threadId]->StopTimer();
				netWork->HandleAddSession(threadId, session);
				});
		}
	};

	u32 threadId = session->GetId() % m_threadCount;
	m_threads[threadId]->PostEvent([this,session, ConnectCB]() {
		i32 ret = uv_tcp_connect(&m_connect, &session->m_socket, (const struct sockaddr*)&(this->m_addr), ConnectCB);
		if (ret < 0)
		{
			ConnectCB(&m_connect, ret);
		}
	});
}

void asUvNetWork::ReConnect(asUvSession* session)
{
	u32 threadId = session->GetId() % m_threadCount;
	m_threads[threadId]->StartTimer([this, session]() {
		this->TryConnect(session);
		}, 10 * 1000);
}

void asUvNetWork::OnAddNewSession(asUvSession& session)
{
}

void asUvNetWork::OnCloseSession(u32 sessionId)
{
}
