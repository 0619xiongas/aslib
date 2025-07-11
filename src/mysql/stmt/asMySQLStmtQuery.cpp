#include "../../../include/mysql/stmt/asMySQLStmtQuery.h"
#include "../../../include/file/asFileReader.h"
#include "../../../include/tools/asStringUtils.hpp"
#ifdef _WIN32
#include "../../../third/cJSON/cJSON.h"
#else
#include <cJSON/cJSON.h>
#endif

asMySQLStmtQuery::asMySQLStmtQuery()
	:m_stmt(this),m_rows(0),m_fields(0),m_affectCount(0),m_insertId(0)
{
	m_results.Init(1024 * 2);
}

asMySQLStmtQuery::~asMySQLStmtQuery()
{
}

bool asMySQLStmtQuery::LoadMySQLStmtMap(const char* path)
{
	asFileReader reader;
	reader.SetPath(path);
	reader.OpenFile();
	reader.ReadAll();
	asBuffer buf = reader.GetInnerBuffer();
	cJSON* root = cJSON_Parse(buf.Buf());
	if (cJSON_IsInvalid(root))
	{
		//log
		cJSON_Delete(root);
		return false;
	}
	if (!cJSON_IsArray(root))
	{
		//log
		cJSON_Delete(root);
		return false;
	}
	bool ret = true;
	i32 size = cJSON_GetArraySize(root);
	for (i32 i = 0;i < size;++i)
	{
		cJSON* node = cJSON_GetArrayItem(root, i);
		if (cJSON_IsInvalid(node) || !cJSON_IsObject(node))
		{
			//log
			ret = false;
			break;
		}
		cJSON* json_id = cJSON_GetObjectItemCaseSensitive(node, "id");
		if (!cJSON_IsString(json_id) || !json_id->valuestring)
		{
			//log
			ret = false;
			break;
		}
		u32 id = astronaut::HexString2UInt32(json_id->valuestring);
		cJSON* json_inParams = cJSON_GetObjectItemCaseSensitive(node, "inParams");
		if (!cJSON_IsString(json_inParams) || !json_inParams->valuestring)
		{
			ret = false;
			break;
		}
		cJSON* json_outParams = cJSON_GetObjectItemCaseSensitive(node, "outParams");
		if (!cJSON_IsString(json_outParams) || !json_outParams->valuestring)
		{
			ret = false;
			break;
		}
		cJSON* json_sql = cJSON_GetObjectItemCaseSensitive(node, "sql");
		if (!cJSON_IsString(json_sql) || !json_sql->valuestring)
		{
			ret = false;
			break;
		}
		cJSON* json_type = cJSON_GetObjectItemCaseSensitive(node, "type");
		if (!cJSON_IsNumber(json_type) || !json_type->valueint)
		{
			ret = false;
			break;
		}
		asMySQLStmt stmt(this);
		if (stmt.InitStmtParams(json_inParams->valuestring,json_outParams->valuestring,json_sql->valuestring,(char)json_type->valueint) != 0)
		{
			ret = false;
			break;
		}
		m_stmts.insert(std::pair<u32, asMySQLStmt>(id,stmt));
	}
	cJSON_Delete(root);
	return ret;
}

i32 asMySQLStmtQuery::PrepareStmtParams(u32 id, const char* in, const char* out, const char* sql, const char flag)
{
	return 0;
}

i32 asMySQLStmtQuery::PrepareStmtParms(u32 id, const char* stmtStr, const char flag)
{
	return 0;
}

i32 asMySQLStmtQuery::Query(const char* in, const char* out, const char* sql, const char flag, i32 num, ...)
{
	return 0;
}

i32 asMySQLStmtQuery::Query(const char* in, const char* out, const char* sql, const char flag, u32 len, const char* data)
{
	return 0;
}

i32 asMySQLStmtQuery::QueryByID(u32 id, i32 num, ...)
{
	return 0;
}

i32 asMySQLStmtQuery::QueryByID(u32 id, u32 len, const char* data)
{
	return 0;
}
