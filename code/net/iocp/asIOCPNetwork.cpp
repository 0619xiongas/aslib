#include "asIOCPNetwork.h"

asIOCPNetwork::asIOCPNetwork()
: m_completionPort(INVALID_HANDLE_VALUE),
  m_listenContext(nullptr),
  m_threadNum(0),
  m_maxRecvBufSize(0),
  m_maxSendBufSize(0),
  m_sessionIDAlloc(1000000),
  m_isStoped(false),
  m_isClient(false),
  m_lpfnAcceptEx(nullptr)
{
}

asIOCPNetwork::~asIOCPNetwork()
{
    if(!m_isStoped)
    {
        StopNetwork();
    }
}

void asIOCPNetwork::Init(const char *ip, u16 port, u32 recvSize, u32 sendSize, u32 threadCount, u32 maxSessionCount, bool isClient)
{
    m_ip = ip;
    m_port = port;
    m_maxRecvBufSize = recvSize;
    m_maxSendBufSize = sendSize;
    m_threadNum = threadCount;
    m_maxSessionCount = maxSessionCount;
    m_isClient = isClient;
}

void asIOCPNetwork::DeInitialize()
{
    if(m_listenContext)
    {
        closesocket(m_listenContext->sock);
        m_listenContext = nullptr;
    }
    if(m_threads.size() > 0)
    {
        for(u32 i = 0; i < m_threadNum; i++)
        {
            CloseHandle(m_threads[i]);
        }
        m_threads.clear();
    }
    if(m_completionPort != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_completionPort);
        m_completionPort = INVALID_HANDLE_VALUE;
    }
}

bool asIOCPNetwork::StartNetwork()
{
    // 初始化完成端口
    m_completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    if(m_completionPort == INVALID_HANDLE_VALUE)
    {
        //log
        return false;
    }
    // 初始化线程
    for(u32 i = 0; i < m_threadNum; i++)
    {
        HANDLE thread = CreateThread(NULL, 0, WorkThreadFunc, (void*)this, 0, NULL);
        if(thread == NULL)
        {
            //log
            // 线程创建失败，清理已创建的资源
            DeInitialize();
            return false;
        }
        m_threads.push_back(thread);
    }
    if(m_isClient)
    {
        ConnectToServer();
    }
    else
    {
        do
        {
            m_listenContext = new asSocketContext();
            m_listenContext->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
            if(m_listenContext->sock == INVALID_SOCKET) break;
            if(CreateIoCompletionPort((HANDLE)m_listenContext->sock, m_completionPort, (ULONG_PTR)m_listenContext, 0) == INVALID_HANDLE_VALUE) break;
            // 绑定地址
            SOCKADDR_IN addr;
            ZeroMemory(&addr, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
            addr.sin_port = htons(m_port);
            if(bind(m_listenContext->sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) break;
            // 监听
            if(listen(m_listenContext->sock, SOMAXCONN) == SOCKET_ERROR) break;
            //get acceptex函数指针
            GUID guidAcceptEx = WSAID_ACCEPTEX;
            DWORD dwBytesReturned = 0;
            if(WSAIoctl(m_listenContext->sock, 
                SIO_GET_EXTENSION_FUNCTION_POINTER, 
                &guidAcceptEx, 
                sizeof(guidAcceptEx), 
                &m_lpfnAcceptEx, 
                sizeof(m_lpfnAcceptEx),
                &dwBytesReturned,
                NULL,
                NULL) == SOCKET_ERROR)
            {
                break;
            }
            // 投递accept事件
            asIOContext* context = new asIOContext(0);
            PostAccept(context);
            return true;
        } while (false);
        // 初始化失败 清理资源
        DeInitialize();
    }
    return false;
}

bool asIOCPNetwork::StopNetwork()
{
    m_isStoped = true;
    
    // 向所有工作线程发送停止信号
    for (u32 i = 0; i < m_threadNum; i++)
    {
        PostQueuedCompletionStatus(m_completionPort, 0, 0, nullptr);
    }
    
    // 等待所有线程退出
    if (m_threads.size() > 0)
    {
        WaitForMultipleObjects(static_cast<DWORD>(m_threads.size()), m_threads.data(), TRUE, INFINITE);
    }
    
    // 关闭所有会话
    for (auto& pair : m_sessionMap)
    {
        delete pair.second;
    }
    m_sessionMap.clear();
    
    // 清理监听socket
    if (m_listenContext)
    {
        delete m_listenContext;
        m_listenContext = nullptr;
    }
    
    // 关闭完成端口
    if (m_completionPort != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_completionPort);
        m_completionPort = INVALID_HANDLE_VALUE;
    }
    
    return true;
}

void asIOCPNetwork::ConnectToServer()
{
    if (m_isClient)
    {
        // 创建客户端socket
        asSocketContext* clientContext = new asSocketContext();
        clientContext->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
        
        if (clientContext->sock == INVALID_SOCKET)
        {
            delete clientContext;
            return;
        }
        
        // 连接到服务器
        SOCKADDR_IN serverAddr;
        ZeroMemory(&serverAddr, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(m_ip.c_str());
        serverAddr.sin_port = htons(m_port);
        
        if (connect(clientContext->sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
        {
            delete clientContext;
            return;
        }
        
        // 创建会话
        u32 sessionID = ++m_sessionIDAlloc;
        asIOCPSession* session = new asIOCPSession();
        session->Init(sessionID, m_maxRecvBufSize, m_maxSendBufSize, this);
        session->m_socketContext = *clientContext;
        
        // 关联到完成端口
        if (CreateIoCompletionPort((HANDLE)clientContext->sock, m_completionPort, (ULONG_PTR)session, 0) == NULL)
        {
            delete session;
            delete clientContext;
            return;
        }
        
        // 添加到会话映射
        m_sessionMap[sessionID] = session;
        
        // 投递接收请求
        asIOContext* recvContext = new asIOContext(m_maxRecvBufSize);
        recvContext->sock = clientContext->sock;
        recvContext->sessionID = sessionID;
        PostRecv(recvContext);
        
        // 调用新会话回调
        OnNewSession(*session);
        
        delete clientContext;
    }
}

void asIOCPNetwork::SendData(u32 sessionID, const char *data, u32 size)
{
    if (!data || size == 0)
    {
        return;
    }
    
    auto itr = m_sessionMap.find(sessionID);
    if (itr == m_sessionMap.end())
    {
        return;
    }
    
    asIOCPSession* session = itr->second;
    
    // 创建共享数据
    AS_SHARED_ARRAY<char> sharedData(new char[size]);
    memcpy(sharedData.get(), data, size);
    
    // 添加到发送队列
    session->m_sendQueue.Push(std::make_pair(sharedData, size));
    
    // 如果没有正在发送的数据，开始发送
    if (!session->m_pending)
    {
        std::pair<AS_SHARED_ARRAY<char>, u32> sendData;
        if (session->m_sendQueue.TryPop(sendData))
        {
            asIOContext* sendContext = new asIOContext(sendData.second);
            sendContext->sock = session->GetStock();
            sendContext->sessionID = sessionID;
            sendContext->type = IET_SEND;
            
            // 复制数据到发送缓冲区
            memcpy(sendContext->buf.buf, sendData.first.get(), sendData.second);
            sendContext->buf.len = sendData.second;
            
            PostSend(sendContext);
            session->m_pending = true;
        }
    }
}

void asIOCPNetwork::SendDataAll(const char *data, u32 size)
{
    if (!data || size == 0)
    {
        return;
    }
    
    for (auto& pair : m_sessionMap)
    {
        SendData(pair.first, data, size);
    }
}

void asIOCPNetwork::SendDataSome(const char *data, u32 size, std::vector<u32> &sessionIDList)
{
    if (!data || size == 0)
    {
        return;
    }
    
    for (u32 sessionID : sessionIDList)
    {
        SendData(sessionID, data, size);
    }
}

void asIOCPNetwork::ConnectToServerAsync()
{
    // 异步连接实现，这里可以创建一个线程来执行连接操作
    // 在实际应用中，可能需要更复杂的异步连接逻辑
    ConnectToServer();
}

void asIOCPNetwork::PostAccept(asIOContext *context)
{
    if(!context)
    {
        return;
    }
    DWORD dwBytes = 0;
    context->type = IET_ACCEPT;
    context->sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if(context->sock == INVALID_SOCKET)
    {
        //log
        delete context;
        return;
    }
    if(false == m_lpfnAcceptEx(m_listenContext->sock, context->sock, context->buf.buf, 
        0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, NULL))
    {
            //log
            closesocket(context->sock);
            context->sock = INVALID_SOCKET;
            delete context;
            return;
    }
    m_listenContext->ioContextList.push_back(context);
}

void asIOCPNetwork::PostRecv(asIOContext *context)
{
    if(!context)
    {
        return;
    }
    DWORD dwBytes = 0, dwFlags = 0;
    context->type = IET_RECV;
    int ret = WSARecv(context->sock, &context->buf, 1, &dwBytes, &dwFlags, &context->overlapped, NULL);
    if(ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
    {
        //log
        auto itr = m_sessionMap.find(context->sessionID);
        if(itr != m_sessionMap.end())
        {
            //log   
            itr->second->Close();
        }
        context->sock = INVALID_SOCKET;
        delete context;
        return;
    }
}

void asIOCPNetwork::PostSend(asIOContext *context)
{
    if(!context)
    {
        return;
    }
    DWORD dwBytes = 0, dwFlags = 0;
    context->type = IET_SEND;
    int ret = WSASend(context->sock, &context->buf, 1, &dwBytes, dwFlags, &context->overlapped, NULL);
    if(ret == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
    {
        //log
        auto itr = m_sessionMap.find(context->sessionID);
        if(itr != m_sessionMap.end())
        {
            //log   
            itr->second->Close();
        }
        context->sock = INVALID_SOCKET;
        delete context;
        return;
    }
}

void asIOCPNetwork::DoAccept(asIOContext *context)
{
    if (!context || context->sock == INVALID_SOCKET)
    {
        return;
    }
    
    // 设置socket选项
    setsockopt(context->sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, 
               (char*)&m_listenContext->sock, sizeof(m_listenContext->sock));
    
    // 创建新的会话
    u32 sessionID = ++m_sessionIDAlloc;
    asIOCPSession* session = new asIOCPSession();
    session->Init(sessionID, m_maxRecvBufSize, m_maxSendBufSize, this);
    
    // 设置会话的socket和地址
    session->m_socketContext.sock = context->sock;
    
    // 获取客户端地址
    int addrLen = sizeof(SOCKADDR_IN);
    getpeername(context->sock, (SOCKADDR*)&session->m_socketContext.addr, &addrLen);
    
    // 将socket关联到完成端口
    if (CreateIoCompletionPort((HANDLE)context->sock, m_completionPort, (ULONG_PTR)session, 0) == NULL)
    {
        delete session;
        closesocket(context->sock);
        return;
    }
    
    // 添加到会话映射
    m_sessionMap[sessionID] = session;
    
    // 投递接收请求
    asIOContext* recvContext = new asIOContext(m_maxRecvBufSize);
    recvContext->sock = context->sock;
    recvContext->sessionID = sessionID;
    PostRecv(recvContext);
    
    // 调用新会话回调
    OnNewSession(*session);
    
    // 重新投递accept请求
    asIOContext* newAcceptContext = new asIOContext(0);
    PostAccept(newAcceptContext);
}

void asIOCPNetwork::DoRecv(asIOContext *context)
{
    if (!context || context->sock == INVALID_SOCKET)
    {
        return;
    }
    
    auto itr = m_sessionMap.find(context->sessionID);
    if (itr == m_sessionMap.end())
    {
        closesocket(context->sock);
        delete context;
        return;
    }
    
    asIOCPSession* session = itr->second;
    
    // 处理接收到的数据
    if (context->buf.len > 0)
    {
        // 将数据写入会话的接收缓冲区
        if (session->m_recvBuffer.WriteBuf(context->buf.buf, context->buf.len))
        {
            // 触发接收数据处理
            session->OnRecvData();
        }
    }
    
    // 重新投递接收请求
    asIOContext* newRecvContext = new asIOContext(m_maxRecvBufSize);
    newRecvContext->sock = context->sock;
    newRecvContext->sessionID = context->sessionID;
    PostRecv(newRecvContext);
    
    // 释放当前上下文
    delete context;
}

void asIOCPNetwork::DoSend(asIOContext *context)
{
    if (!context || context->sock == INVALID_SOCKET)
    {
        return;
    }
    
    auto itr = m_sessionMap.find(context->sessionID);
    if (itr == m_sessionMap.end())
    {
        closesocket(context->sock);
        delete context;
        return;
    }
    
    asIOCPSession* session = itr->second;
    
    // 发送完成，设置pending标志为false
    session->m_pending = false;
    
    // 检查是否还有数据需要发送
    std::pair<AS_SHARED_ARRAY<char>, u32> sendData;
    if (session->m_sendQueue.TryPop(sendData))
    {
        // 还有数据需要发送
        asIOContext* newSendContext = new asIOContext(sendData.second);
        newSendContext->sock = context->sock;
        newSendContext->sessionID = context->sessionID;
        newSendContext->type = IET_SEND;
        
        // 复制数据到发送缓冲区
        memcpy(newSendContext->buf.buf, sendData.first.get(), sendData.second);
        newSendContext->buf.len = sendData.second;
        
        PostSend(newSendContext);
        session->m_pending = true;
    }
    
    // 释放当前上下文
    delete context;
}

void asIOCPNetwork::DoClose(asSocketContext *socketContext)
{
    if (!socketContext)
    {
        return;
    }
    
    // 查找对应的会话
    for (auto it = m_sessionMap.begin(); it != m_sessionMap.end(); ++it)
    {
        if (it->second->GetStock() == socketContext->sock)
        {
            // 调用会话关闭回调
            OnSessionClose(it->first);
            
            // 删除会话
            delete it->second;
            m_sessionMap.erase(it);
            break;
        }
    }
    
    // 释放socket上下文
    delete socketContext;
}

DWORD __stdcall asIOCPNetwork::WorkThreadFunc(LPVOID lpParam)
{
    asIOCPNetwork* network = static_cast<asIOCPNetwork*>(lpParam);
    if (!network)
    {
        return 0;
    }
    
    DWORD bytesTransferred = 0;
    ULONG_PTR completionKey = 0;
    LPOVERLAPPED overlapped = nullptr;
    
    while (!network->m_isStoped)
    {
        BOOL result = GetQueuedCompletionStatus(
            network->m_completionPort,
            &bytesTransferred,
            &completionKey,
            &overlapped,
            INFINITE);
        
        if (result == FALSE)
        {
            // 错误处理
            if (overlapped)
            {
                asIOContext* context = CONTAINING_RECORD(overlapped, asIOContext, overlapped);
                if (context->type == IET_ACCEPT)
                {
                    // accept错误，重新投递
                    network->PostAccept(context);
                }
                else
                {
                    // 其他错误，关闭连接
                    network->DoClose(reinterpret_cast<asSocketContext*>(completionKey));
                    delete context;
                }
            }
            continue;
        }
        
        if (bytesTransferred == 0 && completionKey == 0 && overlapped == nullptr)
        {
            // 停止信号
            break;
        }
        
        if (overlapped)
        {
            asIOContext* context = CONTAINING_RECORD(overlapped, asIOContext, overlapped);
            
            switch (context->type)
            {
            case IET_ACCEPT:
                network->DoAccept(context);
                break;
            case IET_RECV:
                network->DoRecv(context);
                break;
            case IET_SEND:
                network->DoSend(context);
                break;
            case IET_CLOSE:
                network->DoClose(reinterpret_cast<asSocketContext*>(completionKey));
                delete context;
                break;
            default:
                delete context;
                break;
            }
        }
    }
    
    return 0;
}

void asIOCPNetwork::OnNewMessage(asIOCPSession& session, const char* data, u32 size)
{
    // 虚函数，由派生类实现具体的消息处理逻辑
    // 这里可以添加默认的消息处理逻辑

}

void asIOCPNetwork::OnSessionClose(u32 sessionId)
{
    // 虚函数，由派生类实现会话关闭的处理逻辑

}

void asIOCPNetwork::OnNewSession(asIOCPSession& session)
{
    // 虚函数，由派生类实现新会话的处理逻辑

}

bool asIOCPNetwork::ParseData(const char* data, u32 size)
{
    // 虚函数，由派生类实现数据解析逻辑
    // 这里可以添加默认的数据解析逻辑
    if (!data || size == 0)
    {
        return false;
    }
    
    // 简单的数据解析示例：检查数据是否有效
    return (size > 0 && data[0] != 0);
}
