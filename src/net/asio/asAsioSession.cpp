#include "../../../include/net/asio/asAsioSession.h"
#include "../../../include/net/asio/asAsioNetwork.h"
#include "../../../include/log/asLogger.h"

SendQueue::SendQueue():m_size(0)
{
}

SendQueue::~SendQueue()
{
	this->Clear();
}

void SendQueue::Push(AS_SHARED_ARRAY<char>& data, u32 len)
{
	m_que.push_back(std::make_pair(data, len));
	m_size += len;
}

void SendQueue::Pop()
{
	if (!m_que.empty())
	{
		m_size -= m_que.front().second;
		m_que.pop_front();
	}
}

void SendQueue::Clear()
{
	m_que.clear();
	m_size = 0;
}

void SendQueue::MergeQueue2Buf(asBuffer& buffer)
{
	if (m_size)
	{
		for (auto itr : m_que)
		{
			buffer.WriteBuf(itr.first.get(), itr.second);
		}
		Clear();
		m_size = 0;
	}
}


asAsioSession::asAsioSession()
	:m_socket(nullptr),m_id(0), m_network(nullptr),m_pending(false)
{
}

asAsioSession::~asAsioSession()
{
	AS_SAFE_DELETE(m_socket);
}

void asAsioSession::Init(u32 id, boost::asio::ip::tcp::socket* sock, u32 recvBufSize, u32 sendBufSize, asAsioNetwork* network)
{
	m_id = id;
	m_network = network;
	if (!m_socket)
	{
		m_recvBuf.Init(recvBufSize);
		m_socket = sock;
		m_socket->set_option(boost::asio::socket_base::receive_buffer_size(recvBufSize));
		m_socket->set_option(boost::asio::socket_base::send_buffer_size(sendBufSize));
		m_socket->set_option(boost::asio::socket_base::keep_alive(true));
	}
}

u32 asAsioSession::GetID()
{
	return m_id;
}

boost::asio::ip::tcp::socket* asAsioSession::GetSocket()
{
	return m_socket;
}

void asAsioSession::Close()
{
	if (m_network)
	{
		m_network->DoCloseSession(m_id);
	}
}

void asAsioSession::OnReceiveData()
{
	ParseData();
}

void asAsioSession::ParseData()
{
	if (m_recvBuf.Size() > 0)
	{
		if (m_recvBuf.IsFull())
		{
			AS_LOGGER->LogEx(LOGTYPE::BUG, "%u recvBuf is full !!!", m_id);
		}
		char* buf = m_recvBuf.Buf();
		ulint len = m_recvBuf.Size();
		ulint readSize = 0;
		while (1)
		{
			// 长度不足
			if (len < sizeof(asNetTcpMsgHead))
			{
				break;
			}
			// 长度太大
			if (len > AS_NET_MAX_LEN)
			{
				m_recvBuf.Reset(false);
				return;
			}
			asNetTcpMsgHead* head = (asNetTcpMsgHead*)buf;
			// 消息头不对
			if (head->m_flag != AS_NET_TCP_FLAG || head->m_len > AS_NET_MAX_LEN || head->m_len < sizeof(asNetTcpMsgHead))
			{
				m_recvBuf.Reset(false);
				return;
			}
			// 解包机制，len小于头的len 就不继续读了
			if (len < head->m_len)
			{
				break;
			}
			if (m_network)
			{
				char* tmp = new char[head->m_len];
				::memcpy(tmp, head, head->m_len);
				if (tmp)
				{
					m_network->OnNewMessage(*this, head->m_msgId, tmp);
				}
			}
			readSize += head->m_len;
			buf += head->m_len;
			len -= head->m_len;
		}
		m_recvBuf.MoveDataForWard(readSize);
		if (m_recvBuf.Size())
		{
			return;
		}
	}
	return;
}
