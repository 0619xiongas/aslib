// #ifndef AS_IOCPTHREAD_H
// #define AS_IOCPTHREAD_H

// #include "asIOCPSession.h"
// #include <unordered_map>
// #include "../../memory/asMemoryPool.h"
// class asIOCPNetwork;
// class asIOCPThread
// {
//     friend class asIOCPNetwork;
// public:
//     asIOCPThread(HANDLE completionPort);
//     ~asIOCPThread();

//     void ThreadFunc();

//     void SetThreadName(u32 id, const wchar_t* name);

//     void ClearSession();

//     u32 GetSessionCount();

//     asIOCPSession* GetSession(u32 id);

    

// private:
//     HANDLE*     m_completionPort; // 完成端口实例指针
//     std::unordered_map<u32, asIOCPSession*> m_sessions;
//     asMemoryPool<asIOCPSession, 500> m_sessionPool; // session 内存池
//     asMemoryPool<asIOContext, 1000> m_ioContextPool; // ioc 上下文内存池
//     HANDLE m_thread;
//     u32 m_threadId;
// };


// #endif // AS_IOCPTHREAD_H