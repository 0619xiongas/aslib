#ifndef AS_IOCPSESSION_H
#define AS_IOCPSESSION_H

#include "asNetBase.h"
#include "../../buffer/asBuffer.h"
#include <list>
#include <deque>
enum IOCP_EVENT_TYPE
{
    IET_NULL,
    IET_ACCEPT,
    IET_RECV,
    IET_SEND,
    IET_CLOSE,
};
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
struct asSocketContext
{
    SOCKET sock;
    SOCKADDR_IN addr;
    std::list<asIOContext*> ioContextList;
};

class asIOCPNetwork;
class asIOCPSession
{
    friend class asIOCPNetwork;
public:
    asIOCPSession();
    
    virtual ~asIOCPSession();

    void Init(u32 id, u32 recvBufSize, u32 sendBufSize, asIOCPNetwork* network);

    SOCKET GetStock();

    SOCKADDR_IN GetAddr();

    unsigned int GetSessionID();

    void Close();
protected:
    virtual void OnRecvData();

    virtual void ParseData(); //解析包体数据，不同包体分情况解析

protected:
    asSocketContext             m_socketContext;
    asBuffer                    m_recvBuffer; // 接收缓冲区
    bool                        m_pending; // 正在发送中
    std::deque<std::pair<AS_SHARED_ARRAY<char>, u32>> m_sendQueue; // 发送队列
    u64                         m_queSize; // 发送队列大小
    asIOCPNetwork*              m_network;
private:
    unsigned int    m_sessionID;
};

#endif // AS_IOCPSESSION_H
