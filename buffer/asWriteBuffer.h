#pragma once
#include "../asBaseDefine.h"
#include "asBuffer.h"
#include <string>

class asWriteBuffer
{
public:
	asWriteBuffer();
	asWriteBuffer(const asWriteBuffer& wb);
	asWriteBuffer(u32 size, BUFFER_DATA_TYPE type = BUFFER_DATA_TYPE::BDT_NEED_NEW);
	asWriteBuffer(char* data, u32 size);
	~asWriteBuffer();
	void operator= (const asWriteBuffer& other);

	//内部new一个buffer
	bool Set(u32 size, BUFFER_DATA_TYPE type = BUFFER_DATA_TYPE::BDT_NEED_NEW);

	// 外部传入
	bool Set(char* data, u32 size);

	// 跳过一段长度
	inline bool Skip(ulint size)
	{
		return m_buffer.AddSize(size);
	}
	// 缓存地址
	inline char* Buf()
	{
		return m_buffer.Buf();
	}
	// 重新写入
	inline void Reset()
	{
		m_buffer.Reset();
	}
	// 写入的大小
	inline ulint Size()
	{
		return m_buffer.Size();
	}
	// 剩余空间
	inline ulint RemainSize()
	{
		return m_buffer.RemainSize();
	}
	// 最大缓存大小
	inline ulint MaxSize()
	{
		return m_buffer.MaxSize();
	}
	// 当前地址
	inline char* CurrentBuf()
	{
		return m_buffer.CurrentBuf();
	}
	// 重新分配大小
	bool ReSize(ulint size);

	// 释放
	void Clear();

	// Write
	inline bool WriteUChar(u8 data) { return _Write(&data, sizeof(u8)); }
	inline bool WriteChar(i8 data) { return _Write(&data, sizeof(i8)); }
	inline bool WriteI32(i32 data) { return _Write(&data, sizeof(i32)); }
	inline bool WriteU32(u32 data) { return _Write(&data, sizeof(u32)); }
	inline bool WriteFloat(f32 data) { return _Write(&data, sizeof(f32)); }
	inline bool WriteDouble(f64 data) { return _Write(&data, sizeof(f64)); }
	inline bool WriteI64(i64 data) { return _Write(&data, sizeof(i64)); }
	inline bool WriteU64(u64 data) { return _Write(&data, sizeof(u64)); }
	inline bool WriteU16(u16 data) { return _Write(&data, sizeof(u16)); }
	inline bool WriteI16(i16 data) { return _Write(&data, sizeof(i16)); }
	// 不带长度数据
	inline bool WriteData(const void* data, ulint size){return _Write(data, size);}
	inline asWriteBuffer& operator<<(u8 data) { WriteUChar(data); return *this; }
	inline asWriteBuffer& operator<<(i8 data) { WriteChar(data); return *this; }
	inline asWriteBuffer& operator<<(u16 data) { WriteU16(data); return *this; }
	inline asWriteBuffer& operator<<(i16 data) { WriteI16(data); return *this; }
	inline asWriteBuffer& operator<<(u32 data) { WriteU32(data); return *this;}
	inline asWriteBuffer& operator<<(i32 data) { WriteI32(data); return *this; }
	inline asWriteBuffer& operator<<(u64 data) { WriteU64(data); return *this; }
	inline asWriteBuffer& operator<<(i64 data) { WriteI64(data); return *this; }
	inline asWriteBuffer& operator<<(f32 data) { WriteFloat(data); return *this; }
	inline asWriteBuffer& operator<<(f64 data) { WriteDouble(data); return *this;}
	inline asWriteBuffer& operator<<(const std::string& str) { WriteString(str); return *this;}
	inline asWriteBuffer& operator<<(const char* str) { WriteString(str); return *this; }
	// 带四字节长度数据
	bool WriteString(std::string str);
	bool WriteString(const char* data, ulint size);
	bool WriteStruct(const void* data, ulint size);
private:
	void CopyFrom(const asWriteBuffer& wb);
	bool _Write(const void* data, ulint size);
	asBuffer m_buffer;
};
