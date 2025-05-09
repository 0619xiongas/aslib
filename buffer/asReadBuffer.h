#pragma once
#include "../asBaseDefine.h"
#include "asBuffer.h"
#include <string>

// 读取Buffer中的数据
class asReadBuffer
{
public:
	asReadBuffer();
	asReadBuffer(const char* source,ulint size);
	~asReadBuffer();
	// 初始化
	void Set(const char* source, ulint size);
	// 跳过指定大小字节数据
	void Skip(ulint size);
	// 是否是空
	bool IsEmpty();
	inline bool ReadUChar(u8& data)				{ return _Read(&data, sizeof(u8)); }
	inline const u8* ReadUChar()				{ return (const u8*)_Read(sizeof(u8)); }
	inline bool ReadChar(i8& data)				{ return _Read(&data, sizeof(i8)); }
	inline const i8* ReadChar()					{ return (const i8*)_Read(sizeof(i8)); }
	inline bool ReadU32(u32& data)				{ return _Read(&data, sizeof(u32)); }
	inline const u32* ReadU32()					{ return (const u32*)_Read(sizeof(u32)); }
	inline bool ReadI32(i32& data)				{ return _Read(&data, sizeof(i32)); }
	inline const i32* ReadI32()					{ return (const i32*)_Read(sizeof(i32)); }
	inline bool ReadFloat(f32& data)			{ return _Read(&data, sizeof(f32)); }
	inline const f32* ReadFloat()				{ return (const f32*)_Read(sizeof(f32)); }
	inline bool ReadDouble(f64& data)			{ return _Read(&data, sizeof(f64)); }
	inline const f64* ReadDouble()				{ return (const f64*)_Read(sizeof(f64)); }
	inline bool ReadU64(u64& data)				{ return _Read(&data, sizeof(u64)); }
	inline const u64* ReadU64()					{ return (const u64*)_Read(sizeof(u64)); }
	inline bool ReadI64(i64& data)				{ return _Read(&data, sizeof(i64)); }
	inline const i64* ReadI64()					{ return (const i64*)_Read(sizeof(i64)); }
	inline bool ReadU16(u16& data)				{ return _Read(&data, sizeof(u16)); }
	inline const u16* ReadU16()					{ return (const u16*)_Read(sizeof(u16)); }
	inline bool ReadI16(i16& data)				{ return _Read(&data, sizeof(i16)); }
	inline const i16* ReadI16()					{ return (const i16*)_Read(sizeof(i16)); }
	inline bool ReadData(void* data, u32 size)	{ return _Read(data, size); }
	inline const char* ReadData(u32 size)		{ return _Read(size); }
	inline asReadBuffer& operator>>(u8& data) { ReadUChar(data); return *this; }
	inline asReadBuffer& operator>>(i8& data) { ReadChar(data); return *this; }
	inline asReadBuffer& operator>>(u16& data){ ReadU16(data); return *this; }
	inline asReadBuffer& operator>>(i16& data){ ReadI16(data); return *this; }
	inline asReadBuffer& operator>>(u32& data){ ReadU32(data); return *this; }
	inline asReadBuffer& operator>>(i32& data){ ReadI32(data); return *this; }
	inline asReadBuffer& operator>>(u64& data){ ReadU64(data); return *this; }
	inline asReadBuffer& operator>>(i64& data){ ReadI64(data); return *this; }
	inline asReadBuffer& operator>>(f32& data){ ReadFloat(data); return *this; }
	inline asReadBuffer& operator>>(f64& data){ ReadDouble(data); return *this; }
	inline asReadBuffer& operator>>(std::string& str) { ReadString(str); return *this; }


	// ReadString string前带4字节长度
	bool ReadString(std::string& str);
	u32 ReadString(char* data, u32 size);
	u32 ReadStruct(void* data, u32 size);

	inline const char* Buf()
	{
		return m_buf.Buf() + m_pos;
	}
	inline ulint RemainSize()
	{
		return m_buf.Size() - m_pos;
	}
	inline ulint ReadSize()
	{
		return m_pos;
	}
	inline ulint MaxSize()
	{
		return m_buf.MaxSize();
	}

private:
	bool _Read(void* data, ulint size);
	const char* _Read(ulint size);
	asBuffer	m_buf;
	ulint		m_pos;	// 已读的大小
};
