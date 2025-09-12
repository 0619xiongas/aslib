#ifndef AS_IOCPNETWORK_H
#define AS_IOCPNETWORK_H

#include "asIOCPSession.h"
#include <vector>
#include <unordered_map>
#include "../../memory/asMemoryPool.h"
class asIOCPNetwork
{
public:
    asIOCPNetwork();
    virtual ~asIOCPNetwork();
    
    void Init(const char* ip, u16 port, u32 recvSize, u32 sendSize, u32 threadCount, u32 maxSessionCount,bool isClient = false);

    bool StartNetwork();

    bool StopNetwork();

    // 阻塞连接
    void ConnectToServer();

    // 发送数据
    void SendData(u32 sessionID, const char* data, u32 size);
    void SendDataAll(const char* data, u32 size);
    void SendDataSome(const char* data, u32 size, std::vector<u32>& sessionIDList);

protected:
    void StartListen();

private:

    // iocontext
    asIOContext* CreateIOContext();
    void ReleaseIOContext(asIOContext* context);
    
    static DWORD WINAPI WorkThreadFunc(LPVOID lpParam); // iocp工作线程函数
protected:
    HANDLE                  m_completionPort;
    asSocketContext*        m_listenContext; // 监听的socket
    std::vector<HANDLE>     m_threads;
    u32                     m_threadNum;
    u32                     m_maxRecvBufSize;
    u32                     m_maxSendBufSize;
    std::unordered_map<u32, asIOCPSession*> m_sessionMap; // 会话map 暂时不使用内存池
    asMemoryPool<asIOContext, 1000> m_ioContextPool; // ioc上下文内存池 这个不是线程安全的 需要加锁
private:
	bool								m_isClient;
	u32									m_sessionIDAlloc;
	bool								m_isStoped;
    LPFN_ACCEPTEX						m_lpfnAcceptEx; // 监听函数指针
};

#endif // AS_IOCPNETWORK_H