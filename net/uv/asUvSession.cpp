#include "asUvSession.h"
#include <memory>
#include "asUvNetWork.h"
#include "../../buffer/asWriteBuffer.h"

asSendQueue::asSendQueue():m_size(0)
{
}

asSendQueue::~asSendQueue()
{
	this->Clear();
}

void asSendQueue::Push(AS_SHARED_ARRAY<char>& data, u32 len)
{
	m_que.push_back(std::make_pair(data, len));
	m_size += len;
}

void asSendQueue::Pop()
{
	if (!m_que.empty())
	{
		m_size -= m_que.front().second;
		m_que.pop_front();
	}
}

void asSendQueue::Clear()
{
	m_que.clear();
	m_size = 0;
}

void asSendQueue::MergeQueue2Buf(asBuffer& buffer)
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

asUvSession::asUvSession()
	:m_pending(false), m_sessionId(0),m_netWork(nullptr)
{
	::memset(&m_addr, 0, sizeof(m_addr));
	m_req.data = this;
	m_socket.data = this;
}

asUvSession::~asUvSession()
{
}

void asUvSession::Init(u32 id, u32 recvBufSize, u32 sendBufSize, asUvNetWork* net)
{
	m_recvBuf.Init(recvBufSize);
	m_sendBuf.Init(sendBufSize);
	m_netWork = net;
	m_sessionId = id;
}

u32 asUvSession::GetId()
{
	return m_sessionId;
}

void asUvSession::Close()
{
	if (m_netWork)
	{
		m_netWork->DoCloseSession(m_sessionId);
	}
}

void asUvSession::OnClose()
{
	auto FreeFunc = [](uv_handle_t* handle) {
		auto* self = static_cast<asUvSession*>(handle->data);
		if (self)
		{
			delete self; // ʵ�������ڴ˴�����
		}
		};
	uv_read_stop((uv_stream_t*)&m_socket);
	uv_close((uv_handle_t*)&m_socket,FreeFunc);
}

void asUvSession::AllocBuf(uv_buf_t* buf)
{
	buf->base = m_recvBuf.CurrentBuf();
	buf->len = m_recvBuf.RemainSize();
}

// ��������
void asUvSession::OnReceiveData()
{
	// ��߿�����һЩ��Ϣ���ܴ�����⣬��ʱ��̫���˾ͶϿ����� ��ʱ����������Ҫ����ʵ��Ϊ��
	ParseData();
}

void asUvSession::OnWriteDataCB(int ret)
{
	m_sendBuf.Reset(true);
	m_pending = false;
	if (ret)
	{
		//network do
	}
}

void asUvSession::ParseData()
{
	if (m_recvBuf.Size() > 0)
	{
		if (m_recvBuf.IsFull())
		{
			printf("%u recvBuf is full!!!", m_sessionId);
		}
		char* buf = m_recvBuf.Buf();
		ulint len = m_recvBuf.Size();
		ulint readSize = 0;
		while (1)
		{
			// ���Ȳ���
			if (len < sizeof(asNetTcpMsgHead))
			{
				break;
			}
			// ����̫��
			if (len > AS_NET_MAX_LEN)
			{
				m_recvBuf.Reset(false);
				return;
			}
			asNetTcpMsgHead* head = (asNetTcpMsgHead*)buf;
			// ��Ϣͷ����
			if (head->m_flag != AS_NET_TCP_FLAG || head->m_len > AS_NET_MAX_LEN || head->m_len < sizeof(asNetTcpMsgHead))
			{
				m_recvBuf.Reset(false);
				return;
			}
			// ������ƣ�lenС��ͷ��len �Ͳ���������
			if (len < head->m_len)
			{
				break;
			}
			if (m_netWork)
			{
				char* tmp = new char[head->m_len];
				::memcpy(tmp, head, head->m_len);
				if (tmp)
				{
					m_netWork->OnNewMessage(*this, head->m_msgId, tmp);
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
