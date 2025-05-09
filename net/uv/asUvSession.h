#ifndef AS_UVSESSION_H
#define AS_UVSESSION_H
#include "uv.h"
#include <string>
#include "../../buffer/asBuffer.h"
#include <deque>

/*
* 连接信息类，需要做的操作就是维护一个发送数据队列和一个接受数据缓存
*/
struct asSendQueue
{
	asSendQueue();
	~asSendQueue();
	//进队
	void Push(AS_SHARED_ARRAY<char>& data,u32 len);
	// 出队
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

	// 需要通知使用network进行关闭
	void Close();
	void OnClose();
	void AllocBuf(uv_buf_t* buf);
	// uv_read_cb中调用
	void OnReceiveData();
	// uv_write_cb掉用
	void OnWriteDataCB(int ret);
protected:
	virtual void ParseData();

private:
	struct sockaddr_in		m_addr;	//地址
	asBuffer				m_recvBuf; //数据接受缓存
	asBuffer				m_sendBuf; //数据发送
	bool					m_pending; // 是否在发送过程中
	uv_tcp_t				m_socket;// 连接
	asSendQueue				m_sendQueue;//发送队列
	u32						m_sessionId;//连接编号
	asUvNetWork*			m_netWork;
	uv_write_t				m_req; // 发送数据请求
};


#endif // !AS_UVSESSION_H
