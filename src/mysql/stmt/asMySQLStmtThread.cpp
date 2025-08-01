#include "../../../include/mysql/stmt/asMySQLStmtThread.h"
#include "../../../include/log/asLogger.h"
#include "../../../include/time/asTime.h"
#include "../../../include/buffer/asReadBuffer.h"
#include "../../../include/buffer/asNodeBuffer.h"

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

bool asMySQLStmtThread::LoadStmtConfig(u32 id, const char* in, const char* out, const char* sql, const char flag)
{
	if (m_query.PrepareStmtParams(id, in, out, sql, flag) == 0)
	{
		m_map[id] = flag;
		return true;
	}
	return false;
}

bool asMySQLStmtThread::LoadStmtConfig(const char* filePath)
{
	AS_LOGGER->LogEx(TIP, "CALL BASE asMySQLStmtThread::LoadStmtConfig FUNCTION");
	return false;
}

bool asMySQLStmtThread::OnNewMsg(asMsg& msg)
{
	return false;
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

bool asMySQLStmtThread::OnNullRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID)
{
	if (m_query.QueryByID(id, len, data) == 0)
	{
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
		OnNewNetMsg(head, data + AS_MSG_HEAD_SIZE, head->m_len - AS_MSG_HEAD_SIZE, (u32)msg.params[0]);
		AS_SAFE_DELETE_ARRAY(data)
	}
	else
	{
		OnNewMsg(msg);
	}
}
