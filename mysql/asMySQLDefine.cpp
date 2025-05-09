#include "asMySQLDefine.h"

asMySQLQueryResult::asMySQLQueryResult()
	:m_curRes(nullptr), m_columns(0), m_rows(0), m_curRow(nullptr), m_curLen(nullptr),m_resIndex(0), m_success(true)
{
}
asMySQLQueryResult::~asMySQLQueryResult()
{
}

void asMySQLQueryResult::NextResult()
{
	this->FreeCurResult();
	if (m_resIndex >= m_results.size())
	{
		return;
	}
	m_curRes = m_results[m_resIndex];
	m_resIndex++;
	m_columns = mysql_num_fields(m_curRes);
	m_rows = static_cast<u32>(mysql_num_rows(m_curRes));
	mysql_data_seek(m_curRes, 0);
	m_curRow = mysql_fetch_row(m_curRes);
	m_curLen = mysql_fetch_lengths(m_curRes);
}

void asMySQLQueryResult::AddResults(MYSQL_RES* res)
{
	m_results.push_back(res);
}

bool asMySQLQueryResult::IsEof()
{
	return m_curRow == nullptr;
}

bool asMySQLQueryResult::NextRow()
{
	if (m_curRes)
	{
		m_curRow = mysql_fetch_row(m_curRes);
		m_curLen = mysql_fetch_lengths(m_curRes);
		return true;
	}
	return false;
}

bool asMySQLQueryResult::SeekRow(u32 offset)
{
	if (!m_curRes || m_rows < 0 || offset < 0 || m_rows < offset)
	{
		return false;
	}
	mysql_data_seek(m_curRes, offset);
	m_curRow = mysql_fetch_row(m_curRes);
	m_curLen = mysql_fetch_lengths(m_curRes);
	return true;
}

u32 asMySQLQueryResult::GetRowCount()
{
	return m_rows;
}

u32 asMySQLQueryResult::GetColumnCount()
{
	return m_columns;
}

const char* asMySQLQueryResult::GetStringValue(u32 column, const char* defaultStr)
{
	if (!m_curRes || m_columns <= 0 || m_columns < column || !m_curRow || column < 0)
	{
		return defaultStr;
	}

	return m_curRow[column];
}

const char* asMySQLQueryResult::GetStringValue(const char* name, const char* defaultStr)
{
	if (!m_curRes || m_columns <= 0 || !name || !m_curRow)
	{
		return defaultStr;
	}
	u32 col = GetColumnIndex(name);
	return GetStringValue(col, defaultStr);
}

i32 asMySQLQueryResult::GetIntValue(u32 column, const int defaultV)
{
	if (!m_curRes || m_columns <= 0 || m_columns < column || !m_curRow || column < 0)
	{
		return defaultV;
	}
	return ::atoi(m_curRow[column]);
}

i32 asMySQLQueryResult::GetIntValue(const char* name, const int defaultV)
{
	if (!m_curRes || m_columns <= 0 || !name || !m_curRow)
	{
		return defaultV;
	}
	u32 col = GetColumnIndex(name);
	return GetIntValue(col, defaultV);
}

i64 asMySQLQueryResult::GetI64Value(u32 column, const i64 defaultV)
{
	if (!m_curRes || m_columns <= 0 || m_columns < column || !m_curRow || column < 0)
	{
		return defaultV;
	}
	return ::atoll(m_curRow[column]);
}

i64 asMySQLQueryResult::GetI64Value(const char* name, const i64 defaultV)
{
	if (!m_curRes || m_columns <= 0 || !name || !m_curRow)
	{
		return defaultV;
	}
	u32 col = GetColumnIndex(name);
	return GetI64Value(col, defaultV);
}

f64 asMySQLQueryResult::GetDoubleValue(u32 column, const f64 defaultV)
{
	if (!m_curRes || m_columns <= 0 || m_columns < column || !m_curRow || column < 0)
	{
		return defaultV;
	}
	return ::atof(m_curRow[column]);
}

f64 asMySQLQueryResult::GetDoubleValue(const char* name, const f64 defaultV)
{
	if (!m_curRes || m_columns <= 0 || !name || !m_curRow)
	{
		return defaultV;
	}
	u32 col = GetColumnIndex(name);
	return GetDoubleValue(col, defaultV);
}

bool asMySQLQueryResult::GetBlobValue(u32 column, void*& out, unsigned long& len)
{
	if (!m_curRes || m_columns <= 0 || m_columns < column || !m_curRow || column < 0)
	{
		return false;
	}
	len = m_curLen[column];
	out = m_curRow[column];
	return true;
}

bool asMySQLQueryResult::GetBlobValue(const char* name, void*& out, unsigned long& len)
{
	if (!m_curRes || m_columns <= 0 || !name || !m_curRow)
	{
		return false;
	}
	u32 col = GetColumnIndex(name);
	return GetBlobValue(col, out, len);
}

int asMySQLQueryResult::GetColumnIndex(const char* name)
{
	if (!name || !m_curRes || m_columns < 0)
	{
		return -1;
	}
	MYSQL_FIELD* fields = mysql_fetch_fields(m_curRes);
	for (u32 i = 0;i < m_columns;++i)
	{
		if (::strcmp(name, fields[i].name) == 0)
		{
			return i;
		}
	}
	return -1;
}

const char* asMySQLQueryResult::GetColumnName(u32 column, const char* defaultStr)
{
	if (!m_curRes || m_columns < 0 || column < 0 || column > m_columns)
	{
		return defaultStr;
	}
	MYSQL_FIELD* fields = mysql_fetch_fields(m_curRes);
	if (fields)
	{
		return fields[column].name;
	}
	return defaultStr;
}

enum_field_types asMySQLQueryResult::GetColumnType(const char* name)
{
	if (!name || !m_curRes || m_columns < 0)
	{
		return enum_field_types::MYSQL_TYPE_NULL;
	}
	MYSQL_FIELD* fields = mysql_fetch_fields(m_curRes);
	for (u32 i = 0;i < m_columns;++i)
	{
		if (::strcmp(name, fields[i].name) == 0)
		{
			return fields[i].type;
		}
	}
	return enum_field_types::MYSQL_TYPE_NULL;
}

enum_field_types asMySQLQueryResult::GetColumnType(u32 column)
{
	if (!m_curRes || m_columns < 0 || column < 0 || column > m_columns)
	{
		return enum_field_types::MYSQL_TYPE_NULL;
	}
	MYSQL_FIELD* fields = mysql_fetch_fields(m_curRes);
	if (fields)
	{
		return fields[column].type;
	}
	return enum_field_types::MYSQL_TYPE_NULL;
}

std::string& asMySQLQueryResult::GetError()
{
	return m_error;
	// TODO: 在此处插入 return 语句
}

bool asMySQLQueryResult::IsSuccess()
{
	return m_success;
}

bool asMySQLQueryResult::IsEndResults()
{
	return m_resIndex >= m_results.size();
}

void asMySQLQueryResult::FreeResults()
{
	if (m_results.size() > 0)
	{
		for (size_t i = 0;i < m_results.size();++i)
		{
			mysql_free_result(m_results[i]);
		}
	}
	m_results.clear();
	m_curRes = nullptr;
	m_curRow = nullptr;
	m_columns = 0;
	m_rows = 0;
	m_curLen = nullptr;
}

void asMySQLQueryResult::FreeCurResult()
{
	if (m_curRes)
	{
		mysql_free_result(m_curRes);
		m_curRes = nullptr;
		m_curRow = nullptr;
		m_columns = 0;
		m_rows = 0;
		m_curLen = nullptr;
	}
}
