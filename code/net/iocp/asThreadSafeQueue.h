#ifndef AS_THREADSAFEQUEUE_H
#define AS_THREADSAFEQUEUE_H

#include "../asNetBase.h"
#include <queue>
#include <mutex>
#include <condition_variable>

/*
 * @brief 线程安全队列模板类
 */
template<typename T>
class asThreadSafeQueue
{
public:
    asThreadSafeQueue() = default;
    ~asThreadSafeQueue() = default;
    
    // 禁用拷贝构造和赋值
    asThreadSafeQueue(const asThreadSafeQueue&) = delete;
    asThreadSafeQueue& operator=(const asThreadSafeQueue&) = delete;
    
    /*
     * @brief 向队列添加元素
     * @param value 要添加的元素
     */
    void Push(const T& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(value);
        m_condition.notify_one();
    }
    
    /*
     * @brief 向队列添加元素（移动语义）
     * @param value 要添加的元素
     */
    void Push(T&& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_queue.push(std::move(value));
        m_condition.notify_one();
    }
    
    /*
     * @brief 从队列取出元素
     * @param value 接收取出的元素
     * @return 是否成功取出
     */
    bool Pop(T& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty())
        {
            return false;
        }
        value = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }
    
    /*
     * @brief 等待并取出元素（阻塞操作）
     * @param value 接收取出的元素
     */
    void WaitAndPop(T& value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this] { return !m_queue.empty(); });
        value = std::move(m_queue.front());
        m_queue.pop();
    }
    
    /*
     * @brief 尝试取出元素（非阻塞操作）
     * @param value 接收取出的元素
     * @return 是否成功取出
     */
    bool TryPop(T& value)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_queue.empty())
        {
            return false;
        }
        value = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }
    
    /*
     * @brief 获取队列大小
     * @return 队列大小
     */
    size_t Size() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }
    
    /*
     * @brief 检查队列是否为空
     * @return 是否为空
     */
    bool Empty() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }
    
    /*
     * @brief 清空队列
     */
    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        while (!m_queue.empty())
        {
            m_queue.pop();
        }
    }

private:
    mutable std::mutex m_mutex;          // 互斥锁
    std::queue<T> m_queue;               // 底层队列
    std::condition_variable m_condition; // 条件变量
};

#endif // AS_THREADSAFEQUEUE_H