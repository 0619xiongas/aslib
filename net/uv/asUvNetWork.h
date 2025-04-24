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
	// ��Ϊ�ͻ������ӵ�������
	u32 ConnectToServer();
	// �ر�����
	void DoCloseSession(u32 sessionId);
	//�������Ӹ���
	u32 GetSessionCount();

	// ��������
	void DoSendData(char* buf, u32 len, u32 sessionId);
	void DoSendDataAll(char* buf, u32 len);
	void DoSendDataSome(char* buf, u32 len, std::vector<u32>& sessions);

	// ����Ϣ����
	virtual void OnNewMessage(asUvSession& session, u32 msgId, char* buf);

protected:
	// ����
	void StartListen();
	void HandleListen(uv_loop_t* loop);
	// ��������
	void DoAccept(u32 id,int status);
	void HandleAccept(u32 id,uv_loop_t* loop);
	void HandleAddSession(u32 threadId, asUvSession* session);
	// ���������Ϣ
	void StartReceive(asUvSession* session);
	//����ͻ��˹ر�
	void HandleCloseSession(u32 threadId, u32 sessionId);
	// ����������
	void HandleSendData(u32 threadId, u32 sessionId, AS_SHARED_ARRAY<char>buf, u32 len);
	void HandleSendDataAll(u32 threadId, AS_SHARED_ARRAY<char> buf, u32 len);
	void WriteData(asUvThread* pThread, asUvSession* session, AS_SHARED_ARRAY<char> buf,u32 len);
	// ��������server
	void HandleConnectToServer(u32 sessionId,uv_loop_t* loop);
	void TryConnect(asUvSession* session);
	void ReConnect(asUvSession* session);

	// virtual �����ѡ���Լ̳�
	// �µ�session
	virtual void OnAddNewSession(asUvSession& session);
	// �ر�
	virtual void OnCloseSession(u32 sessionId);
protected:
	struct sockaddr_in				m_addr;  //���ص�ַ
	uv_tcp_t						m_socket; // server����socket
	std::vector<asUvThread*>		m_threads;
	u32								m_sendBufSize; // ���ͻ��������ֵ
	u32								m_recvBufSize; // �����������ֵ
	u32								m_threadCount;// �߳�����
	u32								m_sessionCount;// �������session����

private:
	u32								m_sessionIdAlloc;
	bool							m_isClient;			
	uv_connect_t					m_connect; // ����server���� (��Ϊ�ͻ���)
};

#endif
