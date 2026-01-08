#ifdef _WIN32
#ifndef AS_IOCPSESSION_H
#define AS_IOCPSESSION_H

#include "asNetBase.h"
#include "../../buffer/asBuffer.h"
#include "asThreadSafeQueue.h"
#include <deque>
#include <mutex>
#include <condition_variable>
/*
 *@brief iocp会话事件类型
 */
enum IOCP_EVENT_TYPE
{
    IET_NULL,
    IET_ACCEPT,
    IET_RECV,
    IET_SEND,
    IET_CLOSE,
};
/*
 *@brief iocp会话上下文
 */
struct asIOContext
{
    WSAOVERLAPPED overlapped;
    IOCP_EVENT_TYPE type;
    SOCKET sock;
    WSABUF buf;
    u32 sessionID; 

    asIOContext(u_long bufSize)
    {
        ZeroMemory(&overlapped, sizeof(overlapped));
        sock = INVALID_SOCKET;
        type = IET_NULL;
        buf.len = bufSize;
        buf.buf = new char[bufSize];
        memset(buf.buf, 0, bufSize);
        sessionID = 0;
    }
    ~asIOContext()
    {
        if(buf.buf)
        {
            delete[] buf.buf;
            buf.buf = NULL;
        }
    }
    void Reset()
    {
        ZeroMemory(&overlapped, sizeof(overlapped));
        sock = INVALID_SOCKET;
        type = IET_NULL;
        buf.len = 0;
        buf.buf = NULL;
        sessionID = 0;
    }
};
/*
 *@brief iocp会话socket上下文
 */
struct asSocketContext
{
    asSocketContext()
    {
        sock = INVALID_SOCKET;
    }
    ~asSocketContext()
    {
        if(sock != INVALID_SOCKET)
        {
            closesocket(sock);
            sock = INVALID_SOCKET;
        }
        if(ioContextList.size() > 0)
        {
            for(u32 i = 0; i < ioContextList.size(); i++)
            {
                delete ioContextList[i];
            }
            ioContextList.clear();
        }
    }
    SOCKET sock;
    SOCKADDR_IN addr;
    std::deque<asIOContext*> ioContextList;
};

class asIOCPNetwork;
class asIOCPSession
{
    friend class asIOCPNetwork;
public:
    asIOCPSession();
    
    virtual ~asIOCPSession();
    /*
    * @brief 初始化会话
    * @param id 会话id
    * @param recvBufSize 接收缓冲区大小
    * @param sendBufSize 发送缓冲区大小
    * @param network 网络指针
    */
    void Init(u32 id, u32 recvBufSize, u32 sendBufSize, asIOCPNetwork* network);
    /*
    * @brief 获取会话socket
    * @return 会话socket
    */
    SOCKET GetStock();
    /*
    * @brief 获取会话地址
    * @return 会话地址
    */
    SOCKADDR_IN GetAddr();
    /*
    * @brief 获取会话id
    * @return 会话id
    */
    unsigned int GetSessionID();
    /*
    * @brief 关闭会话
    */
    void Close();
    /*
    * @brief 处理接收数据
    */
    void OnRecvData();

protected:
    asSocketContext             m_socketContext;
    asBuffer                    m_recvBuffer; // 接收缓冲区
    bool                        m_pending; // 正在发送中
    asThreadSafeQueue<std::pair<AS_SHARED_ARRAY<char>, u32>> m_sendQueue; // 线程安全的发送队列
    u64                         m_queSize; // 发送队列大小
    std::mutex                  m_sendMutex; // 发送互斥锁
    asIOCPNetwork*              m_network;
private:
    unsigned int    m_sessionID;
};

#endif // AS_IOCPSESSION_H
#endif // _WIN32
