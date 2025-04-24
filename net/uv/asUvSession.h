#ifndef AS_UVSESSION_H
#define AS_UVSESSION_H
#include "uv.h"
#include <string>
#include "../../buffer/asBuffer.h"
#include "../asNetDefine.h"
#include <deque>

/*
* ������Ϣ�࣬��Ҫ���Ĳ�������ά��һ���������ݶ��к�һ���������ݻ���
*/
struct asSendQueue
{
	asSendQueue();
	~asSendQueue();
	//����
	void Push(AS_SHARED_ARRAY<char>& data,u32 len);
	// ����
	void Pop();
	void Clear();
	void MergeQueue2Buf(asBuffer& buffer);
	
	std::deque<std::pair<AS_SHARED_ARRAY<char>, u32>> m_que;
	u64 m_size;
};
class asUvNetWork;
class asUvThread;
class asUvSession
{
	friend class asUvNetWork;
	friend class asUvThread;
public:

	asUvSession();
	~asUvSession();

	void Init(u32 id, u32 recvBufSize, u32 sendBufSize,asUvNetWork* net);

	u32 GetId();

	// ��Ҫ֪ͨʹ��network���йر�
	void Close();
	void OnClose();
	void AllocBuf(uv_buf_t* buf);
	// uv_read_cb�е���
	void OnReceiveData();
	// uv_write_cb����
	void OnWriteDataCB(int ret);
protected:
	virtual void ParseData();

private:
	struct sockaddr_in		m_addr;	//��ַ
	asBuffer				m_recvBuf; //���ݽ��ܻ���
	asBuffer				m_sendBuf; //���ݷ���
	bool					m_pending; // �Ƿ��ڷ��͹�����
	uv_tcp_t				m_socket;// ����
	asSendQueue				m_sendQueue;//���Ͷ���
	u32						m_sessionId;//���ӱ��
	asUvNetWork*			m_netWork;
	uv_write_t				m_req; // ������������
};


#endif // !AS_UVSESSION_H
