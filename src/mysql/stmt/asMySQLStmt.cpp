#include "../../../include/mysql/stmt/asMySQLStmt.h"
#include "../../../include/tools/asStringUtils.hpp"
#include "../../../include/mysql/stmt/asMySQLStmtQuery.h"
#include "../../../include/log/asLogger.h"
using namespace astronaut;
asMySQLStmt::asMySQLStmt(asMySQLStmtQuery* conn)
	:m_query(conn),m_stmt(nullptr), m_inBinds(nullptr),m_outBinds(nullptr),
	m_resFlag(0),m_inCount(0), m_outCount(0), m_inParams(nullptr), m_outParams(nullptr)
{
}

asMySQLStmt::~asMySQLStmt()
{
	Clear();
}

void asMySQLStmt::Clear()
{
	if (m_stmt)
	{
		ClearResult();
		mysql_stmt_close(m_stmt);
		m_stmt = nullptr;
	}
	for (u32 i = 0; i < m_inCount && m_inParams; ++i)
	{
		AS_SAFE_DELETE_ARRAY(m_inBinds);
		AS_SAFE_DELETE(m_inParams[i]);
	}
	AS_SAFE_DELETE_ARRAY(m_inParams);
	for (u32 i = 0; i < m_outCount && m_outParams; ++i)
	{
		AS_SAFE_DELETE_ARRAY(m_outBinds);
		AS_SAFE_DELETE(m_outParams[i]);
	}
	AS_SAFE_DELETE_ARRAY(m_outParams);
	m_resFlag = 0;
	m_inCount = 0;
	m_outCount = 0;
}

void asMySQLStmt::Close()
{
	if (m_stmt)
	{
		ClearResult();
		mysql_stmt_close(m_stmt);
		m_stmt = nullptr;
	}
}

i32 asMySQLStmt::InitStmtParams(const char* in, const char* out, const char* sql, const char flag)
{
	i32 ret = 0;
	m_sql = sql;
	std::vector<std::string> vecIn, vecOut;
	if (in)
	{
		StringSplit(in, ',', vecIn);
		m_inCount = (u32)vecIn.size();
	}
	if (out)
	{
		StringSplit(out, ',', vecOut);
		m_outCount = (u32)vecOut.size();
	}
	m_resFlag = flag;
	if (m_inCount > 0)
	{
		m_inBinds = new MYSQL_BIND[m_inCount];
		::memset(m_inBinds, 0, sizeof(MYSQL_BIND) * m_inCount);
		m_inParams = new asStmtParam * [m_inCount];
		for (u32 i = 0;i < m_inCount;++i)
		{
			if (vecIn[i] == "i32")
			{
				m_inParams[i] = new asStmtI32;
				m_inParams[i]->Bind(m_inBinds[i], 0);
			}
			else if (vecIn[i] == "u32")
			{
				m_inParams[i] = new asStmtU32;
				m_inParams[i]->Bind(m_inBinds[i], 0);
			}
			else if (vecIn[i] == "i8")
			{
				m_inParams[i] = new asStmtChar;
				m_inParams[i]->Bind(m_inBinds[i], 0);
			}
			else if (vecIn[i] == "u8")
			{
				m_inParams[i] = new asStmtUChar;
				m_inParams[i]->Bind(m_inBinds[i], 0);
			}
			else if (vecIn[i] == "i16")
			{
				m_inParams[i] = new asStmtShort;
				m_inParams[i]->Bind(m_inBinds[i], 0);
			}
			else if (vecIn[i] == "u16")
			{
				m_inParams[i] = new asStmtUShort;
				m_inParams[i]->Bind(m_inBinds[i], 0);
			}
			else if (vecIn[i] == "i64")
			{
				m_inParams[i] = new asStmtI64;
				m_inParams[i]->Bind(m_inBinds[i], 0);
			}
			else if (vecIn[i] == "u64")
			{
				m_inParams[i] = new asStmtU64;
				m_inParams[i]->Bind(m_inBinds[i], 0);
			}
			else if (vecIn[i] == "f32")
			{
				m_inParams[i] = new asStmtFloat;
				m_inParams[i]->Bind(m_inBinds[i], 0);
			}
			else if (vecIn[i] == "dt")
			{
				m_inParams[i] = new asStmtTime;
				m_inParams[i]->Bind(m_inBinds[i], 0);
			}
			else if (vecIn[i][0] == 's')
			{
				m_inParams[i] = new asStmtString;
				u32 len = String2UInt32(vecIn[i].c_str() + 1);
				m_inParams[i]->Bind(m_inBinds[i], len);
			}
			else if (vecIn[i][0] == 'b')
			{
				m_inParams[i] = new asStmtBlob;
				u32 len = String2UInt32(vecIn[i].c_str() + 1);
				m_inParams[i]->Bind(m_inBinds[i], len);
			}
			else
			{
				AS_LOGGER->LogEx(ERR, "asMySQLStmt::InitStmtParams can't find inParams type,type is %s", vecIn[i].c_str());
				ret = -1;
				break;
			}
		}
	}
	if (m_outCount > 0)
	{
		m_outBinds = new MYSQL_BIND[m_outCount];
		::memset(m_outBinds, 0, sizeof(MYSQL_BIND) * m_outCount);
		m_outParams = new asStmtParam * [m_outCount];
		for (u32 i = 0;i < m_outCount;++i)
		{
			if (vecOut[i] == "i32")
			{
				m_outParams[i] = new asStmtI32;
				m_outParams[i]->Bind(m_outBinds[i], 0);
			}
			else if (vecOut[i] == "u32")
			{
				m_outParams[i] = new asStmtU32;
				m_outParams[i]->Bind(m_outBinds[i], 0);
			}
			else if (vecOut[i] == "i8")
			{
				m_outParams[i] = new asStmtChar;
				m_outParams[i]->Bind(m_outBinds[i], 0);
			}
			else if (vecOut[i] == "u8")
			{
				m_outParams[i] = new asStmtUChar;
				m_outParams[i]->Bind(m_outBinds[i], 0);
			}
			else if (vecOut[i] == "i16")
			{
				m_outParams[i] = new asStmtShort;
				m_outParams[i]->Bind(m_outBinds[i], 0);
			}
			else if (vecOut[i] == "u16")
			{
				m_outParams[i] = new asStmtUShort;
				m_outParams[i]->Bind(m_outBinds[i], 0);
			}
			else if (vecOut[i] == "i64")
			{
				m_outParams[i] = new asStmtI64;
				m_outParams[i]->Bind(m_outBinds[i], 0);
			}
			else if (vecOut[i] == "u64")
			{
				m_outParams[i] = new asStmtU64;
				m_outParams[i]->Bind(m_outBinds[i], 0);
			}
			else if (vecOut[i] == "f32")
			{
				m_outParams[i] = new asStmtFloat;
				m_outParams[i]->Bind(m_outBinds[i], 0);
			}
			else if (vecOut[i] == "dt")
			{
				m_outParams[i] = new asStmtTime;
				m_outParams[i]->Bind(m_outBinds[i], 0);
			}
			else if (vecOut[i][0] == 's')
			{
				m_outParams[i] = new asStmtString;
				u32 len =String2UInt32(vecOut[i].c_str() + 1);
				m_outParams[i]->Bind(m_outBinds[i], len);
			}
			else if (vecOut[i][0] == 'b')
			{
				m_outParams[i] = new asStmtBlob;
				u32 len = String2UInt32(vecOut[i].c_str() + 1);
				m_outParams[i]->Bind(m_outBinds[i], len);
			}
			else
			{
				AS_LOGGER->LogEx(ERR, "asMySQLStmt::InitStmtParams can't find outParams type,type is %s", vecOut[i].c_str());
				ret = -1;
				break;
			}
		}
	}
	if(ret)
	{
		return ret;
	}
	return PrepareStmt();
}

i32 asMySQLStmt::PrepareStmt()
{
	if(!m_query)
	{
		return -1;
	}
	if(m_sql.empty())
	{
		return -1;
	}
	m_stmt = mysql_stmt_init(m_query->m_mysql.m_conn);
	if(!m_stmt)
	{
		AS_LOGGER->Log(ERR, "asMySQLStmt::PrepareStmt, mysql_stmt_init failed");
		return -1;
	}
	i32 ret = mysql_stmt_prepare(m_stmt, m_sql.c_str(), (unsigned long)m_sql.length());
	if(ret != 0)
	{
		AS_LOGGER->LogEx(ERR, "asMySQLStmt::PrepareStmt, mysql_stmt_prepare failed, reason is %s", mysql_stmt_error(m_stmt));
		return -1;
	}
	return 0;
}

i32 asMySQLStmt::ExecuteQuery(u32 num, ...)
{
	i32 ret = 0;
	va_list va;
	va_start(va, num);
	ret = ExecuteQuery(num, va);
	va_end(va);
	return ret;
}

i32 asMySQLStmt::ExecuteQuery(u32 num, va_list& args)
{
	if (num < m_inCount)
	{
		AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num < m_inCount, num is %u, m_inCount is %u", num, m_inCount);
		return -1;
	}
	ResetBindData();
	if (m_inCount > 0)
	{
		for (u32 i = 0;i < m_inCount;++i)
		{
			switch (m_inParams[i]->m_type)
			{
			case '1':
			{
				int value = va_arg(args, int);
				m_inParams[i]->SetData(&value, 0);
				if (--num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			case '2':
			{
				unsigned int value = va_arg(args, unsigned int);
				m_inParams[i]->SetData(&value, 0);
				if (--num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			case '3':
			{

				char c = va_arg(args, int);
				m_inParams[i]->SetData(&c, 0);
				if (--num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			case '4':
			{
				unsigned char c = va_arg(args, unsigned int);
				m_inParams[i]->SetData(&c, 0);
				if (--num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			case '5':
			{
				short s = va_arg(args, int);
				m_inParams[i]->SetData(&s, 0);
				if (--num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			case '6':
			{
				unsigned short s = va_arg(args, unsigned int);
				m_inParams[i]->SetData(&s, 0);
				if (--num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			case '7':
			{
				long long value = va_arg(args, long long);
				m_inParams[i]->SetData(&value, 0);
				if (--num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			case '8':
			{
				unsigned long long value = va_arg(args, unsigned long long);
				m_inParams[i]->SetData(&value, 0);
				if (--num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			case '9':
			{
				asStmtTime* t = dynamic_cast<asStmtTime*>(m_inParams[i]);
				YMDHMS* tt = reinterpret_cast<YMDHMS*>(va_arg(args, void*));
				t->SetData(tt->year.u32p, tt->month.u32p, tt->day.u32p, tt->hour.u32p, tt->minutes.u32p, tt->second.u32p);
				if (--num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			case 'a':
			case 'b':
			{
				unsigned int len = va_arg(args, unsigned int);
				char* str = va_arg(args, char*);
				m_inParams[i]->SetData(str, len);
				num -= 2;
				if (num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			case 'c':
			{
				float f = (float)va_arg(args, double);
				m_inParams[i]->SetData(&f, 0);
				if (--num < 0)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, num = 0, i = %d", i);
					return -1;
				}
			}
			break;
			default:
				AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, error type, type is %c", m_inParams[i]->m_type);
				return -1;
			}
		}
	}
	i32 ret = 0;
	if (m_inCount > 0)
	{
		ret = Execute(m_inBinds);
	}
	else
	{
		ret = Execute(nullptr);
	}
	return ret;
}

i32 asMySQLStmt::ExecuteQuery(u32 len, const char* data)
{
	char* pData = const_cast<char*>(data);
	u32 Len = len;
	ResetBindData();
	if (m_inCount > 0)
	{
		for (u32 i = 0; i < m_inCount; ++i)
		{
			switch (m_inParams[i]->m_type)
			{
			case '1':
			{
				m_inParams[i]->SetData(pData, 0);
				if (Len < sizeof(int))
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				pData += sizeof(int);
				Len -= sizeof(int);
			}
			break;
			case '2':
			{
				m_inParams[i]->SetData(pData, 0);
				if (Len < sizeof(unsigned int))
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				pData += sizeof(unsigned int);
				Len -= sizeof(unsigned int);
			}
			break;
			case '3':
			{
				m_inParams[i]->SetData(pData, 0);
				if (Len < sizeof(char))
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				pData += sizeof(char);
				Len -= sizeof(char);
			}
			break;
			case '4':
			{
				m_inParams[i]->SetData(pData, 0);
				if (Len < sizeof(unsigned char))
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				pData += sizeof(unsigned char);
				Len -= sizeof(unsigned char);
			}
			break;
			case '5':
			{
				m_inParams[i]->SetData(pData, 0);
				if (Len < sizeof(short))
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				pData += sizeof(short);
				Len -= sizeof(short);
			}
			break;
			case '6':
			{
				m_inParams[i]->SetData(pData, 0);
				if (Len < sizeof(unsigned short))
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				pData += sizeof(unsigned short);
				Len -= sizeof(unsigned short);
			}
			break;
			case '7':
			{
				m_inParams[i]->SetData(pData, 0);
				if (Len < sizeof(long long))
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				pData += sizeof(long long);
				Len -= sizeof(long long);
			}
			break;
			case '8':
			{
				m_inParams[i]->SetData(pData, 0);
				if (Len < sizeof(unsigned long long))
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				pData += sizeof(unsigned long long);
				Len -= sizeof(unsigned long long);
			}
			break;
			case '9':
			{
				asStmtTime* t = dynamic_cast<asStmtTime*>(m_inParams[i]);
				YMDHMS* tt = reinterpret_cast<YMDHMS*>(pData);
				if (Len < sizeof(YMDHMS))
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				t->SetData(tt->year.u32p, tt->month.u32p, tt->day.u32p, tt->hour.u32p, tt->minutes.u32p, tt->second.u32p);
				pData += sizeof(YMDHMS);
				Len -= sizeof(YMDHMS);
			}
			break;
			case 'a':
			case 'b':
			{
				unsigned int strLen = astronaut::String2UInt32(pData);
				pData += sizeof(unsigned int);
				m_inParams[i]->SetData(pData, strLen);
				if (Len < sizeof(unsigned int) + strLen)
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				pData += strLen;
				Len -= (strLen + sizeof(unsigned int));
			}
			break;
			case 'c':
			{
				m_inParams[i]->SetData(pData, 0);
				if (Len < sizeof(float))
				{
					AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, Len is not enough, i = %d", i);
					return -1;
				}
				pData += sizeof(float);
				Len -= sizeof(float);
			}
			break;
			default:
				AS_LOGGER->LogEx(ERR, "asMySQLStmt::ExecuteQuery, error type, type is %c", m_inParams[i]->m_type);
				return -1;
			}
		}
	}
	i32 ret = 0;
	if (m_inCount > 0)
	{
		ret = Execute(m_inBinds);
	}
	else
	{
		ret = Execute(nullptr);
	}
	return ret;
}

u8 asMySQLStmt::GetQueryType()
{
	return m_resFlag;
}

void asMySQLStmt::ClearResult()
{
	if (m_stmt)
	{
		mysql_stmt_free_result(m_stmt);
		while (mysql_stmt_next_result(m_stmt) == 0)
		{
			mysql_stmt_free_result(m_stmt);
		}
	}
}

void asMySQLStmt::ResetBindData()
{
	for (u32 i = 0; i < m_inCount; ++i)
	{
		m_inParams[i]->Reset();
	}
	for (u32 i = 0; i < m_outCount; ++i)
	{
		m_outParams[i]->Reset();
	}
}

i32 asMySQLStmt::Execute(MYSQL_BIND* binds)
{
	if (binds)
	{
		if (mysql_stmt_bind_param(m_stmt, binds))
		{
			AS_LOGGER->LogEx(ERR, "asMySQLStmt::Execute,mysql_stmt_bind_param failed, reason is %s", mysql_stmt_error(m_stmt));
			return -1;
		}
	}
	if (mysql_stmt_execute(m_stmt))
	{
		AS_LOGGER->LogEx(ERR, "asMySQLStmt::Execute,mysql_stmt_execute failed, reason is %s", mysql_stmt_error(m_stmt));
		return -1;
	}
	m_query->m_affectCount = (u32)mysql_stmt_affected_rows(m_stmt);
	m_query->m_insertId = (m_resFlag & AS_DB_STMT_RESULT_INSERT) ? (u64)mysql_stmt_insert_id(m_stmt) : 0;
	return 0;
}

i32 asMySQLStmt::GetResultsToBuffer()
{
	m_query->m_rows = 0;
	m_query->m_results.Empty();
	if ((m_resFlag & AS_DB_STMT_RESULT_ONE) || (m_resFlag & AS_DB_STMT_RESULT_MULTI))
	{
		if (mysql_stmt_bind_result(m_stmt, m_outBinds))
		{
			AS_LOGGER->LogEx(ERR, "asMySQLStmt::GetResultsToBuffer,mysql_stmt_bind_result failed, reason is %s", mysql_stmt_error(m_stmt));
			return -1;
		}
		if (mysql_stmt_store_result(m_stmt))
		{
			AS_LOGGER->LogEx(ERR, "asMySQLStmt::GetResultsToBuffer,mysql_stmt_store_result failed, reason is %s", mysql_stmt_error(m_stmt));
			return -1;
		}
		u64 rows = mysql_stmt_num_rows(m_stmt);
		if (rows > 0)
		{
			while (!mysql_stmt_fetch(m_stmt))
			{
				m_query->m_results.StartWrite();
				for (u32 i = 0; i < m_outCount; ++i)
				{
					switch (m_outParams[i]->m_type)
					{
					case '1':
						m_query->m_results.Write(m_outParams[i]->GetData(), sizeof(int));
						break;
					case '2':
						m_query->m_results.Write(m_outParams[i]->GetData(), sizeof(unsigned int));
						break;
					case '3':
						m_query->m_results.Write(m_outParams[i]->GetData(), sizeof(char));
						break;
					case '4':
						m_query->m_results.Write(m_outParams[i]->GetData(), sizeof(unsigned char));
						break;
					case '5':
						m_query->m_results.Write(m_outParams[i]->GetData(), sizeof(short));
						break;
					case '6':
						m_query->m_results.Write(m_outParams[i]->GetData(), sizeof(unsigned short));
						break;
					case '7':
						m_query->m_results.Write(m_outParams[i]->GetData(), sizeof(long long));
						break;
					case '8':
						m_query->m_results.Write(m_outParams[i]->GetData(), sizeof(unsigned long long));
						break;
					case '9':
					{
						asStmtTime* t = dynamic_cast<asStmtTime*>(m_outParams[i]);
						YMDHMS tt; tt.Zero();
						t->GetData(tt);
						m_query->m_results.Write(&tt, sizeof(tt));
					}
					break;
					case 'a':
					case 'b':
						m_query->m_results.Write(m_outParams[i]->GetData(), m_outParams[i]->m_len);
						break;
					case 'c':
						m_query->m_results.Write(m_outParams[i]->GetData(), sizeof(float));
					default:
						AS_LOGGER->LogEx(ERR, "asMySQLStmt::GetResultsToBuffer,error type, type is %c", m_outParams[i]->m_type);
						return -1;
					}
				}
				m_query->m_results.EndWrite();
				++m_query->m_rows;
			}
		}
	}
	return 0;
}