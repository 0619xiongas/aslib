#ifndef AS_MYSQLDEFINE_H
#define AS_MYSQLDEFINE_H

#include "../asBaseDefine.h"
#ifdef _WIN32
#include <mysql.h>
#else
#include <mysql/mysql.h>
#define my_bool bool
#endif
#include <memory>
#include "../buffer/asBuffer.h"
#include "../time/asTime.h"

enum class asMySQLCmdType
{
	query,
	execute,
};

/*
int -> i32
unsigned int -> u32
char -> i8
unsigned char -> u8
short - > i16
unsigned short -> u16
long long int -> i64
unsigned long long int -> u64
time -> dt
string -> s##len
blob -> b##len
float -> f32
*/

struct asStmtParam
{
public:
	asStmtParam():m_type(0),m_len(0),m_null(1){}
	virtual ~asStmtParam(){}
	virtual void Bind(MYSQL_BIND& bind,unsigned long len) = 0;
	virtual const void* GetData() = 0;
	virtual void SetData(void* data,unsigned long len) = 0;
	virtual void Reset() = 0;
	/* data */
protected:
	char			m_type;
	unsigned long 	m_len;
	my_bool			m_null;
};

struct asStmtI32 : public asStmtParam
{
public:
	asStmtI32() :asStmtParam()
	{
		m_type = '1';
		value = 0;
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		bind.buffer_type = MYSQL_TYPE_LONG;
		bind.buffer = &value;
		bind.length = &m_len;
		bind.buffer_length = sizeof(int);
		bind.is_null = &m_null;
		bind.is_unsigned = 0;
	}
	virtual const void* GetData() override
	{
		return &value;
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			value = *(int*)data;
			m_null = 0;
		}
		else
		{
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value = 0;
	}
private:
	i32 value;
};

struct asStmtU32 : public asStmtParam
{
public:
	asStmtU32() :asStmtParam()
	{
		m_type = '2';
		value = 0;
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		bind.buffer_type = MYSQL_TYPE_LONG;
		bind.buffer = &value;
		bind.length = &m_len;
		bind.buffer_length = sizeof(unsigned int);
		bind.is_null = &m_null;
		bind.is_unsigned = 1;
	}
	virtual const void* GetData() override
	{
		return &value;
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			value = *(unsigned int*)data;
			m_null = 0;
		}
		else
		{
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value = 0;
	}
private:
	unsigned int value;
};

struct asStmtChar : public asStmtParam
{
public:
	asStmtChar() :asStmtParam()
	{
		m_type = '3';
		value = 0;
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		bind.buffer_type = MYSQL_TYPE_TINY;
		bind.buffer = &value;
		bind.length = &m_len;
		bind.buffer_length = sizeof(char);
		bind.is_null = &m_null;
		bind.is_unsigned = 0;
	}
	virtual const void* GetData() override
	{
		return &value;
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			value = *(char*)data;
			m_null = 0;
		}
		else
		{
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value = 0;
	}
private:
	char value;
};

struct asStmtUChar : public asStmtParam
{
public:
	asStmtUChar() :asStmtParam()
	{
		m_type = '4';
		value = 0;
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		bind.buffer_type = MYSQL_TYPE_TINY;
		bind.buffer = &value;
		bind.length = &m_len;
		bind.buffer_length = sizeof(unsigned char);
		bind.is_null = &m_null;
		bind.is_unsigned = 1;
	}
	virtual const void* GetData() override
	{
		return &value;
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			value = *(unsigned char*)data;
			m_null = 0;
		}
		else
		{
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value = 0;
	}
private:
	unsigned char value;
};

struct asStmtShort : public asStmtParam
{
public:
	asStmtShort() :asStmtParam()
	{
		m_type = '5';
		value = 0;
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		bind.buffer_type = MYSQL_TYPE_SHORT;
		bind.buffer = &value;
		bind.length = &m_len;
		bind.buffer_length = sizeof(short);
		bind.is_null = &m_null;
		bind.is_unsigned = 0;
	}
	virtual const void* GetData() override
	{
		return &value;
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			value = *(short*)data;
			m_null = 0;
		}
		else
		{
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value = 0;
	}
private:
	short value;
};

struct asStmtUShort : public asStmtParam
{
public:
	asStmtUShort() :asStmtParam()
	{
		m_type = '6';
		value = 0;
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		bind.buffer_type = MYSQL_TYPE_SHORT;
		bind.buffer = &value;
		bind.length = &m_len;
		bind.buffer_length = sizeof(unsigned short);
		bind.is_null = &m_null;
		bind.is_unsigned = 1;
	}
	virtual const void* GetData() override
	{
		return &value;
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			value = *(unsigned short*)data;
			m_null = 0;
		}
		else
		{
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value = 0;
	}
private:
	unsigned short value;
};

struct asStmtI64 : public asStmtParam
{
public:
	asStmtI64() :asStmtParam()
	{
		m_type = '7';
		value = 0;
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		bind.buffer_type = MYSQL_TYPE_LONGLONG;
		bind.buffer = &value;
		bind.length = &m_len;
		bind.buffer_length = sizeof(long long);
		bind.is_null = &m_null;
		bind.is_unsigned = 0;
	}
	virtual const void* GetData() override
	{
		return &value;
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			value = *(long long*)data;
			m_null = 0;
		}
		else
		{
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value = 0;
	}
private:
	long long value;
};

struct asStmtU64 : public asStmtParam
{
public:
	asStmtU64() :asStmtParam()
	{
		m_type = '8';
		value = 0;
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		bind.buffer_type = MYSQL_TYPE_LONGLONG;
		bind.buffer = &value;
		bind.length = &m_len;
		bind.buffer_length = sizeof(unsigned long long);
		bind.is_null = &m_null;
		bind.is_unsigned = 1;
	}
	virtual const void* GetData() override
	{
		return &value;
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			value = *(unsigned long long*)data;
			m_null = 0;
		}
		else
		{
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value = 0;
	}
private:
	unsigned long long value;
};

struct asStmtTime : public asStmtParam
{
public:
	asStmtTime() :asStmtParam()
	{
		m_type = '9';
		Reset();
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		bind.buffer_type = MYSQL_TYPE_DATETIME;
		bind.buffer = &value;
		bind.length = 0;
		bind.buffer_length = sizeof(MYSQL_TIME);
		bind.is_null = &m_null;
		bind.is_unsigned = 0;
	}
	virtual const void* GetData() override
	{
		return &value;
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			::memcpy(&time, data, len);
			m_null = 0;
		}
		else
		{
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		::memset(&value, 0, sizeof(MYSQL_TIME));
	}
	void SetData(unsigned int year, unsigned int mon, unsigned int day, unsigned int hour, unsigned int min, unsigned int sec)
	{
		value.year = year;
		value.month = mon;
		value.day = day;
		value.hour = hour;
		value.minute = min;
		value.second = sec;
		value.neg = false;
		value.time_type = MYSQL_TIMESTAMP_DATETIME;
		m_null = 0;
	}
	void GetData(YMDHMS& t)
	{
		t.year.u32p = value.year;
		t.month.u32p = value.month;
		t.day.u32p = value.day;
		t.hour.u32p = value.hour;
		t.minutes.u32p = value.minute;
		t.second.u32p = value.second;
	}
private:
	MYSQL_TIME value;
};

struct asStmtString : public asStmtParam
{
public:
	asStmtString() :asStmtParam()
	{
		m_type = 'a';
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		value.Init(len);
		bind.buffer_type = MYSQL_TYPE_STRING;
		bind.buffer = value.Buf();
		bind.length = &m_len;
		bind.buffer_length = len;
		bind.is_null = &m_null;
		bind.is_unsigned = 0;
	}
	virtual const void* GetData() override
	{
		return value.Buf();
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			m_len = len;
			value.WriteBuf(data, len);
			m_null = 0;
		}
		else
		{
			m_len = 0;
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value.Reset();
		m_len = 0;
	}
private:
	asBuffer value;
};

struct asStmtBlob : public asStmtParam
{
public:
	asStmtBlob() :asStmtParam()
	{
		m_type = 'b';
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		value.Init(len);
		bind.buffer_type = MYSQL_TYPE_BLOB;
		bind.buffer = value.Buf();
		bind.length = &m_len;
		bind.buffer_length = len;
		bind.is_null = &m_null;
		bind.is_unsigned = 0;
	}
	virtual const void* GetData() override
	{
		return value.Buf();
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			m_len = len;
			value.WriteBuf(data, len);
			m_null = 0;
		}
		else
		{
			m_len = 0;
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value.Reset();
		m_len = 0;
	}
private:
	asBuffer value;
};

struct asStmtFloat : public asStmtParam
{
public:
	asStmtFloat() :asStmtParam()
	{
		m_type = 'c';
	}
	virtual void Bind(MYSQL_BIND& bind, unsigned long len) override
	{
		bind.buffer_type = MYSQL_TYPE_FLOAT;
		bind.buffer = &value;
		bind.length = &m_len;
		bind.buffer_length = sizeof(float);
		bind.is_null = &m_null;
		bind.is_unsigned = 0;
	}
	virtual const void* GetData() override
	{
		return &value;
	}
	virtual void SetData(void* data, unsigned long len) override
	{
		if (data)
		{
			value = *(float*)data;
			m_null = 0;
		}
		else
		{
			m_null = 1;
		}
	}
	virtual void Reset() override
	{
		value = 0.0f;
	}
private:
	float	value;
};
#endif