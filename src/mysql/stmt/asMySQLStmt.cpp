#include "../../../include/mysql/stmt/asMySQLStmt.h"
#include "../../../include/tools/asStringUtils.hpp"
#include "../../../include/mysql/stmt/asMySQLStmtQuery.h"
using namespace astronaut;
asMySQLStmt::asMySQLStmt(asMySQLStmtQuery* conn)
{
}

asMySQLStmt::~asMySQLStmt()
{
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
				// log
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
				// log
				ret = -1;
				break;
			}
		}
	}
	if(ret)
	{
		return ret
	}
	return PrepareStmt();
}

i32 asMySQLStmt::PrepareStmt()
{
	if(!m_Mgr)
	{
		return -1;
	}
	if(m_sql.empty())
	{
		return -1;
	}
	m_stmt = mysql_stmt_init(m_Mgr->m_mysql.m_conn);
	if(!m_stmt)
	{
		//log
		return -1;
	}
	if(mysql_stmt_prepare(m_stmt,m_sql.c_str(),m_sql.length()) != 0)
	{
		// log
		return -1;
	}
	return 0;
}

i32 asMySQLStmt::ExecuteQuery(i32 num, ...)
{

}