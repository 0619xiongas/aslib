#include "../../../include/mysql/stmt/asMySQLStmtQuery.h"
#include "../../../include/log/asLogger.h"
#include "../../../include/buffer/asReadBuffer.h"
asMySQLStmtQuery::asMySQLStmtQuery()
	:m_stmt(this),m_rows(0),m_fields(0),m_affectCount(0),m_insertId(0)
{
	m_results.Init(1024 * 2);
}

asMySQLStmtQuery::~asMySQLStmtQuery()
{
}
i32 asMySQLStmtQuery::PrepareStmtParams(u32 id, const char* stmtStr, const char flag)
{
	std::string stmt(stmtStr);
	auto paramFunc = [](std::string& out, std::string stmt, std::string param, size_t& pos)
		{
			size_t start = stmt.find(param, pos);
			if (start != std::string::npos)
			{
				start = start + 2;
				size_t end = stmt.find("}", pos);
				if (end != std::string::npos)
				{
					out = stmt.substr(start, end - start);
					pos = end + 1;
				}
			}
		};
	size_t pos = 0;
	std::string in = "";
	std::string out = "";
	std::string sql = "";
	paramFunc(in, stmt, "@{", pos);
	paramFunc(out, stmt, "#{", pos);
	paramFunc(sql, stmt, "${", pos);
	return PrepareStmtParams(id,in.c_str(),out.c_str(),sql.c_str(),flag);
}

i32 asMySQLStmtQuery::PrepareStmtParams(u32 id, const char* in, const char* out, const char* sql, const char flag)
{
	asMySQLStmt stmt(this);
	auto ret = m_stmts.insert(std::pair<u32, asMySQLStmt>(id, stmt));
	if (ret.second)
	{
		if (ret.first->second.InitStmtParams(in, out, sql, flag) != 0)
		{
			return -1;
		}
	}
	else
	{
		AS_LOGGER->LogEx(ERR, "asMySQLStmtQuery::PrepareStmtParams, insert into map failed, id is %u", id);
		return -1;
	}
	return 0;
}

i32 asMySQLStmtQuery::Query(const char* in, const char* out, const char* sql, const char flag, i32 num, ...)
{
	m_stmt.Clear();
	i32 ret = m_stmt.InitStmtParams(in, out, sql, flag);
	if (ret != 0)
	{
		return ret;
	}
	va_list va;
	va_start(va, num);
	ret = m_stmt.ExecuteQuery(num, va);
	va_end(va);
	if (!ret)
	{
		m_stmt.GetResultsToBuffer();
	}
	m_stmt.ClearResult();
	return ret;
}

i32 asMySQLStmtQuery::Query(const char* in, const char* out, const char* sql, const char flag, u32 len, const char* data)
{
	m_stmt.Clear();
	i32 ret = m_stmt.InitStmtParams(in, out, sql, flag);
	if (ret != 0)
	{
		return ret;
	}
	ret = m_stmt.ExecuteQuery(len, data);
	if (!ret)
	{
		m_stmt.GetResultsToBuffer();
	}
	m_stmt.ClearResult();
	return ret;
}

i32 asMySQLStmtQuery::QueryByID(u32 id, i32 num, ...)
{
	auto itr = m_stmts.find(id);
	if (itr == m_stmts.end())
	{
		return -1;
	}
	va_list va;
	va_start(va, num);
	i32 ret = itr->second.ExecuteQuery(num, va);
	va_end(va);
	if (!ret)
	{
		itr->second.GetResultsToBuffer();
	}
	itr->second.ClearResult();
	return ret;
}

i32 asMySQLStmtQuery::QueryByID(u32 id, u32 len, const char* data)
{
	auto itr = m_stmts.find(id);
	if (itr == m_stmts.end())
	{
		return -1;
	}
	i32 ret = itr->second.ExecuteQuery(len, data);
	if (!ret)
	{
		itr->second.GetResultsToBuffer();
	}
	itr->second.ClearResult();
	return ret;
}

void asMySQLStmtQuery::GetOneResult(asReadBuffer& rb)
{
	auto itr = m_results.Begin();
	while (itr.IsInvaild())
	{
		rb.Set(itr.Data(), itr.Size());
	}
}

u8 asMySQLStmtQuery::GetQueryType(u32 id)
{
	auto itr = m_stmts.find(id);
	if (itr != m_stmts.end())
	{
		return itr->second.GetQueryType();
	}
	return 0;
}
