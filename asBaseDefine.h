#ifndef AS_BASEDEFINE_H
#define AS_BASEDEFINE_H

typedef int					i32;
typedef short				i16;
typedef char				i8;
typedef unsigned int		u32;
typedef unsigned short		u16;
typedef unsigned char		u8;
typedef long long int		i64;
typedef unsigned long long int	u64;
typedef float				f32;
typedef double				f64;
typedef i64					lint;
typedef u64					ulint;
typedef int					bool32;

#if __cplusplus >= 201103L
#define std11
#endif

#ifdef std11
#include <memory>
#define AS_SHARED_ARRAY std::shared_ptr
#define AS_SHARED_PTR std::shared_ptr
#define AS_MAKE_SHARED	std::make_shared
#else
#include "boost/smart_ptr.hpp"
#define AS_SHARED_ARRAY boost::shared_array
#define AS_SHARED_PTR boost::shared_ptr
#define AS_MAKE_SHARED boost::make_shared
#endif
template<typename T>
inline AS_SHARED_ARRAY<T> make_shared_array(unsigned int size)
{
#ifdef std11
	return std::shared_ptr<T>(new T[size], std::default_delete<T[]>());
#else
	return boost::shared_array<T>(new T[size]);
#endif
}
template<typename T>
inline AS_SHARED_ARRAY<T> make_shared_array(T* ptr)
{
#ifdef std11
	return std::shared_ptr<T>(ptr, std::default_delete<T[]>());
#else
	return boost::shared_array<T>(ptr);
#endif
}

#define SAFE_DELETE(p) if(p){delete p; p = nullptr;}
#define SAFE_DELETE_ARRAY(p) if(p){delete[] p; p = nullptr;}


union Union32
{
	i8 i8p;
	u8 u8p;
	i16 i16p;
	u16 u16p;
	i32 i32p;
	u32 u32p;
	f32 f32p;
};

union Union64
{
	i8 i8p;
	u8 u8p;
	i16 i16p;
	u16 u16p;
	i32 i32p;
	u32 u32p;
	i64 i64p;
	u64 u64p;
	f32 f32p;
	f64 f64p;
	void* pp;
	Union32 union32p;
};

#define AS_NET_TCP_FLAG 0x20250422
#define AS_NET_MAX_LEN 1073741824
#define AS_NET_MSG_DIFF 0x10000000

struct asNetTcpMsgHead
{
	asNetTcpMsgHead():m_flag(AS_NET_TCP_FLAG),m_len(0),m_msgId(0)
	{}
	u32		m_flag;//消息头标志
	u32		m_len; // 消息体长度，包括head
	u32		m_msgId;//消息编号
};

#endif