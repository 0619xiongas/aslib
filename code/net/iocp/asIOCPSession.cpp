#include "asIOCPSession.h"
#include "asIOCPNetwork.h"
#include <iostream>

asIOCPSession::asIOCPSession()
    : m_pending(false)
    , m_queSize(0)
    , m_network(nullptr)
    , m_sessionID(0)
{
}

asIOCPSession::~asIOCPSession()
{
    Close();
}

void asIOCPSession::Init(u32 id, u32 recvBufSize, u32 sendBufSize, asIOCPNetwork* network)
{
    m_sessionID = id;
    m_network = network;
    
    // 初始化接收缓冲区
    if (!m_recvBuffer.Init(recvBufSize))
    {
        // 初始化失败，记录日志
        std::cout << "Failed to initialize receive buffer for session " << id << std::endl;
        return;
    }
    
    // 初始化发送队列大小
    m_queSize = 0;
    m_pending = false;
    
    std::cout << "Session " << id << " initialized successfully" << std::endl;
}

SOCKET asIOCPSession::GetStock()
{
    return m_socketContext.sock;
}

SOCKADDR_IN asIOCPSession::GetAddr()
{
    return m_socketContext.addr;
}

unsigned int asIOCPSession::GetSessionID()
{
    return m_sessionID;
}

void asIOCPSession::Close()
{
    std::lock_guard<std::mutex> lock(m_sendMutex);
    
    if (m_socketContext.sock != INVALID_SOCKET)
    {
        closesocket(m_socketContext.sock);
        m_socketContext.sock = INVALID_SOCKET;
    }
    
    // 清空发送队列
    m_sendQueue.Clear();
    m_queSize = 0;
    m_pending = false;
    
    std::cout << "Session " << m_sessionID << " closed" << std::endl;
}

void asIOCPSession::OnRecvData()
{
    // 这里需要处理接收到的数据
    // 在实际实现中，这里应该解析数据包并调用网络层的回调函数
    
    // 示例：将接收到的数据传递给网络层处理
    if (m_network && m_recvBuffer.Size() > 0)
    {
        m_network->OnNewMessage(*this, m_recvBuffer.Buf(), static_cast<u32>(m_recvBuffer.Size()));
        
        // 清空接收缓冲区，准备接收下一批数据
        m_recvBuffer.Init(m_recvBuffer.MaxSize());
    }
}