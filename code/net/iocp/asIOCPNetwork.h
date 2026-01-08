#ifdef _WIN32
#ifndef AS_IOCPNETWORK_H
#define AS_IOCPNETWORK_H

#include "asIOCPSession.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "../../memory/asMemoryPool.h"
class asIOCPNetwork
{
public:
    asIOCPNetwork();
    virtual ~asIOCPNetwork();
    
    /*
    * @brief 初始化网络
    * @param ip 服务器ip
    * @param port 服务器端口
    * @param recvSize 接收缓冲区大小
    * @param sendSize 发送缓冲区大小
    * @param threadCount 工作线程数量
    * @param maxSessionCount 最大会话数量
    * @param isClient 是否作为客户端
    */
    void Init(const char* ip, u16 port, u32 recvSize, u32 sendSize, u32 threadCount, u32 maxSessionCount,bool isClient = false);
    /*
    * @brief 清理网络以及所有资源
    */
    void DeInitialize();
    /*
    * @brief 启动网络
    * @return 是否成功
    */
    bool StartNetwork();
    /*
    * @brief 停止网络
    * @return 是否成功
    */
    bool StopNetwork();

    /*
    * @brief 连接到服务器
    */
    void ConnectToServer();
    /*
    * @brief 轮询连接到服务器
    */
    void ConnectToServerAsync();

    /*
    * @brief 发送数据
    * @param sessionID 会话id
    * @param data 数据
    * @param size 数据大小
    */
    void SendData(u32 sessionID, const char* data, u32 size);
    /*
    * @brief 发送数据给所有会话
    * @param data 数据
    * @param size 数据大小
    */
    void SendDataAll(const char* data, u32 size);
    /*
    * @brief 发送数据给指定会话列表
    * @param data 数据
    * @param size 数据大小
    * @param sessionIDList 会话id列表
    */
    void SendDataSome(const char* data, u32 size, std::vector<u32>& sessionIDList);

    /*
    * @brief 收到新消息
    * @param session 会话
    * @param data 数据
    * @param size 数据大小
    */
    virtual void OnNewMessage(asIOCPSession& session, const char* data, u32 size);
protected:
    /*
    * @brief 投递一个接受新会话请求
    * @param context 上下文
    */
    void PostAccept(asIOContext* context);
    /*
    * @brief 投递一个接收数据请求
    * @param context 上下文
    */
    void PostRecv(asIOContext* context);
    /*
    * @brief 投递一个发送数据请求
    * @param context 上下文
    */
    void PostSend(asIOContext* context);

    /*
    * @brief 执行一个接受新会话请求
    * @param context 上下文
    */
    void DoAccept(asIOContext* context);
    /*
    * @brief 执行一个接收数据请求
    * @param context 上下文
    */
    void DoRecv(asIOContext* context);
    /*
    * @brief 执行一个发送数据请求
    * @param context 上下文
    */
    void DoSend(asIOContext* context);
    /*
    * @brief 执行关闭会话
    * @param socketContext 会话上下文
    */
    void DoClose(asSocketContext* socketContext);
    /*
    * @brief 会话关闭回调
    * @param sessionId 会话id
    */
    virtual void OnSessionClose(u32 sessionId);
    /*
    * @brief 新会话回调
    * @param session 会话
    */
    virtual void OnNewSession(asIOCPSession& session);
    /*
    * @brief 解析包体数据，不同包体分情况解析
    * @param data 数据
    * @param size 数据大小
    * @return 是否成功
    */
    virtual bool ParseData(const char* data, u32 size);
private:
    /*
    * @brief iocp工作线程函数
    * @param lpParam 参数
    * @return 线程id
    */
    static DWORD WINAPI WorkThreadFunc(LPVOID lpParam);
protected:
    HANDLE                  m_completionPort;// 完成端口
    asSocketContext*        m_listenContext; // 监听的socket
    std::vector<HANDLE>     m_threads;// 工作线程句柄
    u32                     m_threadNum;// 工作线程数量
    u32                     m_maxRecvBufSize;// 最大接收缓冲区大小
    u32                     m_maxSendBufSize;// 最大发送缓冲区大小
    u32                     m_maxSessionCount;// 最大会话数量
    std::string             m_ip;// ip
    u16                     m_port;// 端口
    std::unordered_map<u32, asIOCPSession*> m_sessionMap; // 会话map 暂时不使用内存池
private:
	bool								m_isClient; // 是否作为客户端
	u32									m_sessionIDAlloc; // 会话id分配器
	bool								m_isStoped; // 是否停止
    LPFN_ACCEPTEX						m_lpfnAcceptEx; // 监听函数指针
};

#endif // AS_IOCPNETWORK_H
#endif // _WIN32