#ifndef AS_ASIONETWORK_H
#define AS_ASIONETWORK_H
#include "asAsioThread.h"

class asAsioNetwork
{
public:
	asAsioNetwork();
	virtual ~asAsioNetwork();

	void Init(const char* ip, i32 port, u32 sendBufSize, u32 recvBufSize, u32 threadCount, u32 sessionCount,std::string netName="");

	bool TryRunNetwork(bool isClient);

	bool TryStopNetwork();
	// 作为客户端连接到服务器 阻塞连接
	u32 ConnectToServer();
	// 关闭连接
	void DoCloseSession(u32 sessionId);
	//返回连接个数
	u32 GetSessionCount();
	// 发送数据
	void DoSendData(char* buf, u32 len, u32 sessionId);
	void DoSendDataAll(char* buf, u32 len);
	void DoSendDataSome(char* buf, u32 len, std::vector<u32>& sessions);

	// 处理网络消息，子类需重写
	virtual void OnNewMessage(asAsioSession& session, u32 msgId, char* buf);
	//  设置网络服务名
	void SetNetworkName(const char* name, const wchar_t* wName);
protected:
	void DoAccept();

	void HandleAccept(u32 sessionID, boost::asio::ip::tcp::socket* sock, const boost::system::error_code& ec);

	void HandleAddSession(u32 threadID, asAsioSession* sessionPtr);

	void StartReceive(asAsioSession* sessionPtr);

	void HandleReceived(const boost::system::error_code& ec, size_t bytestransfered, asAsioSession* sessionPtr);

	void HandleCloseSession(u32 threadID, u32 sessionID);

	void HandleSendData(u32 threadID,u32 sessionID, AS_SHARED_ARRAY<char>buf, u32 len);

	void HandleSendDataAll(u32 threadID, AS_SHARED_ARRAY<char>buf, u32 len);

	void WriteData(asAsioThread* pThread, asAsioSession* sessionPtr, AS_SHARED_ARRAY<char> buf, u32 len);

	void SendSessionQueue(asAsioThread* pThread, asAsioSession* sessionPtr);

	void HandleWrited(const boost::system::error_code& ec, size_t bytestransfered, asAsioSession* sessionPtr, AS_SHARED_ARRAY<char> buf);

	virtual void OnAddNewSession(asAsioSession& session);
	// 关闭
	virtual void OnCloseSession(u32 sessionId);
public:
	std::string							m_name; // 网络服务名
protected:
	boost::asio::ip::tcp::endpoint		m_addr;
	boost::asio::ip::tcp::acceptor*		m_acceptor;
	asAsioThread**						m_threads;
	u32									m_threadNum;
	u32									m_maxSessionCount;
	u32									m_recvBufSize;
	u32									m_sendBufSize;
private:
	bool								m_isClient;
	u32									m_sessionIDAlloc;
	bool								m_isStoped;
};

#endif
