#include "../../../include/mysql/stmt/asMySQLStmtThread.h"
#include "../../../include/log/asLogger.h"
#include "../../../include/file/asFileReader.h"
#include "../../../include/tools/asStringUtils.hpp"
#include "../../../include/time/asTime.h"
#include "../../../include/buffer/asReadBuffer.h"
#include "../../../include/buffer/asNodeBuffer.h"
#ifdef _WIN32
#include "../../../third/cJSON/cJSON.h"
#else
#include <cJSON/cJSON.h>
#endif
asMySQLStmtThread::asMySQLStmtThread()
{
	m_log = false;
}

asMySQLStmtThread::~asMySQLStmtThread()
{
}

bool asMySQLStmtThread::Init(const char* host, u16 port, const char* user, const char* pwd, const char* db, const char* character,bool log)
{
	m_log = log;
	return m_query.m_mysql.Init(host, port, user, pwd, db, character);
}

bool asMySQLStmtThread::LoadStmtConfig(const char* filePath)
{
	asFileReader reader;
	reader.SetPath(filePath);
	reader.OpenFile();
	reader.ReadAll();
	asBuffer buf = reader.GetInnerBuffer();
	cJSON* root = cJSON_Parse(buf.Buf());
	if (cJSON_IsInvalid(root))
	{
		AS_LOGGER->Log(ERR, "asMySQLStmtThread::LoadStmtConfig, file is not a json data");
		cJSON_Delete(root);
		return false;
	}
	if (!cJSON_IsArray(root))
	{
		AS_LOGGER->Log(ERR, "asMySQLStmtThread::LoadStmtConfig, file is not a json array data");
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
			AS_LOGGER->Log(ERR, "asMySQLStmtThread::LoadStmtConfig, node is not a json object");
			ret = false;
			break;
		}
		cJSON* json_id = cJSON_GetObjectItemCaseSensitive(node, "id");
		if (!cJSON_IsString(json_id) || !json_id->valuestring)
		{
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
		if (m_query.PrepareStmtParams(id, json_inParams->valuestring, json_outParams->valuestring, json_sql->valuestring, (char)json_type->valueint) != 0)
		{
			ret = false;
			break;
		}
		m_map[id] = (char)json_type->valueint;
	}
	cJSON_Delete(root);
	return ret;
}

bool asMySQLStmtThread::OnNewNetMsg(asNetMsgHead* head, const char* data, u32 len, u32 connectID)
{
	auto itr = m_map.find(head->m_msgId);
	if (itr == m_map.end())
	{
		AS_LOGGER->LogEx(ERR, "asMySQLStmtThread::OnNewNetMsg, no this %u msgID in stmt map", head->m_msgId);
		return false;
	}
	char type = itr->second;
	u64 begin = 0;
	u64 end = 0;
	bool ret = 0;
	if (m_log)
	{
		begin = asTime::GetTimeNowMs().u64p;
	}
	if (type & AS_DB_STMT_RESULT_NULL)
	{
		ret = OnNullRecord(head->m_msgId, head, data, len, connectID);
	}
	else if (type & AS_DB_STMT_RESULT_ONE)
	{
		ret = OnOneRecord(head->m_msgId, head, data, len, connectID);
	}
	else if (type & AS_DB_STMT_RESULT_MULTI)
	{
		ret = OnMultiRecord(head->m_msgId, head, data, len, connectID);
	}
	if (m_log)
	{
		end = asTime::GetTimeNowMs().u64p;
		AS_LOGGER->LogEx(TIP, "stmt thread [%d],msg [%x],used %llu ms", m_threadId, head->m_msgId, end - begin);
	}
	return ret;
}

bool asMySQLStmtThread::OnNewMsg(asMsg& msg)
{
	return false;
}

bool asMySQLStmtThread::OnNullRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID)
{
	if (m_query.QueryByID(id, len, data) == 0)
	{
		// ²åÈë·µ»Ø
		if (m_query.GetQueryType(id) & AS_DB_STMT_RESULT_INSERT)
		{
			u32 rows = m_query.m_affectCount;
			u32 insertId = (u32)m_query.m_insertId;
			ulint* param = (ulint*)(data + len - sizeof(ulint));

			asDBNetParam res;
			res.param = *param;
			res.rows = rows;
			res.insertID = insertId;
			u32 sendLen = sizeof(asNetMsgHead) + sizeof(asDBNetParam);
			head->m_len = sendLen;
			char* sendBuf = new char[sendLen];

			::memcpy(sendBuf, head, AS_MSG_HEAD_SIZE);
			::memcpy(sendBuf + AS_MSG_HEAD_SIZE, &res, sizeof(asDBNetParam));
			SendResponse(connectID, (asNetMsgHead*)sendBuf, sendBuf, sendLen);
			return true;
		}
		return false;
	}
	return false;
}

bool asMySQLStmtThread::OnOneRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID)
{
	if (m_query.QueryByID(id, len, data) == 0)
	{
		asReadBuffer rb;
		m_query.GetOneResult(rb);
		u32 rows = (u32)m_query.m_results.Count();
		ulint* param = (ulint*)(data + len - sizeof(ulint));
		
		asDBNetParam res;
		res.param = *param;
		res.rows = rows;
		u32 sendLen = sizeof(asNetMsgHead) + sizeof(asDBNetParam) + (u32)rb.MaxSize();
		head->m_len = sendLen;
		char* sendBuf = new char[sendLen];

		::memcpy(sendBuf, head, AS_MSG_HEAD_SIZE);
		::memcpy(sendBuf + AS_MSG_HEAD_SIZE, &res, sizeof(asDBNetParam));
		if (!rb.IsEmpty())
		{
			::memcpy(sendBuf + AS_MSG_HEAD_SIZE + sizeof(asDBNetParam), rb.Buf(), rb.MaxSize());
		}
		SendResponse(connectID, (asNetMsgHead*)sendBuf, sendBuf, sendLen);
		return true;
	}
	return false;
}

bool asMySQLStmtThread::OnMultiRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID)
{
	if (m_query.QueryByID(id, len, data) == 0)
	{
		asNodeBuffer& nb = m_query.GetResults();
		u32 rows = (u32)m_query.m_results.Count();
		ulint* param = (ulint*)(data + len - sizeof(ulint));

		asDBNetParam res;
		res.param = *param;
		res.rows = rows;
	}
	return false;
}

void asMySQLStmtThread::HandleMessage(asMsg& msg)
{
	if (msg.msgID == AS_THREADMSG_NET_RECV)
	{
		asNetMsgHead* head = (asNetMsgHead*)msg.params[1];
		char* data = (char*)msg.params[1];
		OnNewNetMsg(head, data + AS_MSG_HEAD_SIZE, head->m_len - AS_MSG_HEAD_SIZE, msg.params[0]);
		AS_SAFE_DELETE_ARRAY(data)
	}
	else
	{
		OnNewMsg(msg)
	}
}
