#ifndef AS_UVNETWORK_H
#define AS_UVNETWORK_H

#include "asUvThread.h"
#include <vector>
#include <functional>

class asUvNetWork
{
public:

	asUvNetWork();
	virtual ~asUvNetWork();

	void Init(const char* ip,i32 port,u32 sendBufSize,u32 recvBufSize,u32 threadCount,u32 sessionCount);

	bool TryRunNetWork(bool isClient);

	bool TryStopNetWork();
	// 作为客户端连接到服务器
	u32 ConnectToServer();
	// 关闭连接
	void DoCloseSession(u32 sessionId);
	//返回连接个数
	u32 GetSessionCount();

	// 发送数据
	void DoSendData(char* buf, u32 len, u32 sessionId);
	void DoSendDataAll(char* buf, u32 len);
	void DoSendDataSome(char* buf, u32 len, std::vector<u32>& sessions);

	// 有消息发来
	virtual void OnNewMessage(asUvSession& session, u32 msgId, char* buf);

protected:
	// 监听
	void StartListen();
	void HandleListen(uv_loop_t* loop);
	// 处理连接
	void DoAccept(u32 id,int status);
	void HandleAccept(u32 id,uv_loop_t* loop);
	void HandleAddSession(u32 threadId, asUvSession* session);
	// 处理接收消息
	void StartReceive(asUvSession* session);
	//处理客户端关闭
	void HandleCloseSession(u32 threadId, u32 sessionId);
	// 处理发送数据
	void HandleSendData(u32 threadId, u32 sessionId, AS_SHARED_ARRAY<char>buf, u32 len);
	void HandleSendDataAll(u32 threadId, AS_SHARED_ARRAY<char> buf, u32 len);
	void WriteData(asUvThread* pThread, asUvSession* session, AS_SHARED_ARRAY<char> buf,u32 len);
	// 处理连接server
	void HandleConnectToServer(u32 sessionId,uv_loop_t* loop);
	void TryConnect(asUvSession* session);
	void ReConnect(asUvSession* session);

	// virtual 子类可选择性继承
	// 新的session
	virtual void OnAddNewSession(asUvSession& session);
	// 关闭
	virtual void OnCloseSession(u32 sessionId);
protected:
	struct sockaddr_in				m_addr;  //本地地址
	uv_tcp_t						m_socket; // server监听socket
	std::vector<asUvThread*>		m_threads;
	u32								m_sendBufSize; // 发送缓冲区最大值
	u32								m_recvBufSize; // 接受数据最大值
	u32								m_threadCount;// 线程数量
	u32								m_sessionCount;// 最大连接session数量

private:
	u32								m_sessionIdAlloc;
	bool							m_isClient;			
	uv_connect_t					m_connect; // 连接server参数 (作为客户端)
};

#endif
