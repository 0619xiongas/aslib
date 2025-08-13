#ifndef AS_ASIOSESSION_H
#define AS_ASIOSESSION_H

#include <boost/asio.hpp>
#include "../../buffer/asBuffer.h"
#include <deque>

struct SendQueue
{
	SendQueue();
	~SendQueue();
	//进队
	void Push(AS_SHARED_ARRAY<char>& data, u32 len);
	// 出队
	void Pop();
	void Clear();
	void MergeQueue2Buf(asBuffer& buffer);

	std::deque<std::pair<AS_SHARED_ARRAY<char>, u32>> m_que;
	u64 m_size;
};

class asAsioNetwork;
class asAsioSession
{
	friend class asAsioNetwork;
public:
	asAsioSession();

	virtual ~asAsioSession();

	void Init(u32 id, boost::asio::ip::tcp::socket* sock,u32 recvBufSize, u32 sendBufSize, asAsioNetwork* network);

	u32 GetID();

	boost::asio::ip::tcp::socket* GetSocket();

	void Close();

	void OnReceiveData();

protected:
	virtual void ParseData();
protected:
	boost::asio::ip::tcp::socket*		m_socket;
	asBuffer					m_recvBuf;
	u32							m_id;
	SendQueue					m_sendQueue;
	bool						m_pending;
	asAsioNetwork*				m_network;
};

#endif // !AS_ASIOSESSION_H
