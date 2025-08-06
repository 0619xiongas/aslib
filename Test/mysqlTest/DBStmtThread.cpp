#include "DBStmtThread.h"
#include <iostream>
#include "file/asFileReader.h"
#include "cJSON.h"
#include "log/asLogger.h"
#include "tools/asStringUtils.hpp"
#include "SQLProducer.h"
DBStmtThread::DBStmtThread()
{
}

DBStmtThread::~DBStmtThread()
{
}

bool DBStmtThread::SendResponse(u32 connectID, asNetMsgHead* head, char* data, u32 len)
{
	asSingleton<SQLProducer>::instance()->OnSQL_CB(head, data, len);
	AS_SAFE_DELETE_ARRAY(data);
	return false;
}

bool DBStmtThread::LoadStmtConfig(const char* filePath)
{
	asFileReader reader;
	reader.SetPath(filePath);
	reader.OpenFile();
	reader.ReadAll();
	asBuffer buf = reader.GetInnerBuffer();
	cJSON* root = cJSON_Parse(buf.Buf());
	if (cJSON_IsInvalid(root))
	{
		AS_LOGGER->Log(ERR, "DBStmtThread::LoadStmtConfig, file is not a json data");
		cJSON_Delete(root);
		return false;
	}
	if (!cJSON_IsArray(root))
	{
		AS_LOGGER->Log(ERR, "DBStmtThread::LoadStmtConfig, file is not a json array data");
		cJSON_Delete(root);
		return false;
	}
	bool ret = true;
	i32 size = cJSON_GetArraySize(root);
	for (i32 i = 0; i < size; ++i)
	{
		cJSON* node = cJSON_GetArrayItem(root, i);
		if (cJSON_IsInvalid(node) || !cJSON_IsObject(node))
		{
			AS_LOGGER->Log(ERR, "DBStmtThread::LoadStmtConfig, node is not a json object");
			ret = false;
			break;
		}
		cJSON* json_id = cJSON_GetObjectItemCaseSensitive(node, "id");
		if (!cJSON_IsString(json_id) || !json_id->valuestring)
		{
			ret = false;
			break;
		}
		u32 id = HexString2UInt32(json_id->valuestring);
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
		char* in = strlen(json_inParams->valuestring) > 0 ? json_inParams->valuestring : nullptr;
		char* out = strlen(json_outParams->valuestring) > 0 ? json_outParams->valuestring : nullptr;
		if (m_query.PrepareStmtParams(id, in, out, json_sql->valuestring, (char)json_type->valueint) != 0)
		{
			ret = false;
			break;
		}
		m_map[id] = (char)json_type->valueint;
	}
	cJSON_Delete(root);
	return ret;
}
