#ifndef AS_NODEBUFFER_H
#define AS_NODEBUFFER_H

#include "asBuffer.h"

/*
* 假设这是asBuffer的数据:
* [-----------------------------------------------]
* [*******xxxxxxxxx*******xxxxxxxxx*******xxxxxxxx]
* [<-len-><- data-><-len-><- data-><-len-><-data->]
*/

class asNodeBuffer
{
    class asNBIterator
    {
        friend class asNodeBuffer;
    public:
        asNBIterator() :pData(nullptr), count(0), size(0) {}
        asNBIterator(const asNBIterator& other) :pData(other.pData), count(other.count), size(other.size) {}
        ~asNBIterator()
        {
            Clear();
        }
        void operator=(const asNBIterator& other)
        {
            pData = other.pData;
            count = other.count;
            size = other.size;
        }
        void Clear()
        {
            pData = nullptr;
            count = 0;
            size = 0;
        }
        ulint Size() const
        {
            return size;
        }
        const char* Data()
        {
            return pData;
        }
        bool IsInvaild() const
        {
            return pData != nullptr;
        }
    private:
        char* pData;
        ulint count;
        ulint size;
    };
public:
    asNodeBuffer();
    asNodeBuffer(const asNodeBuffer& other);
    ~asNodeBuffer();
    void operator = (const asNodeBuffer& other);
    // 初始化
    bool Init(ulint size);
    // 解析这段buf，不管理buf的析构
    bool FormatFrom(char* buf, ulint len);
    // 数据段个数
    ulint Count() const;
    //使用的内存大小
    ulint Size() const;
    // 剩余大小
    ulint RemainSize() const;
    // 返回m_buf的地址
    const char* Buf() const;
    // 清空缓存 不释放内存
    void Empty();
    // 迭代器
    asNBIterator Begin();
    bool Next(asNBIterator& itr);
    // 释放内存
    void Clear();
    // 开始写入 调用Write函数必须用此
    void StartWrite();
    // 写入
    bool Write(const void* buf, ulint len);
    // 写入序列化的数据(写入前会写入这段数据的长度)
    bool WriteSerialize‌(const void* buf, ulint len);
    // 停止写入这段数据 Start 和 End 成对出现
    void EndWrite();
private:
    void CopyFrom(const asNodeBuffer& other);
private:
    asBuffer m_buf;
    ulint    m_count; // 数据段的个数
    ulint    m_offset; //当前buf的偏移量
};

#endif