#include "DBStmtThreadGroup.h"
#include "log/asLogger.h"
DBStmtThreadGroup::DBStmtThreadGroup()
{
}

DBStmtThreadGroup::~DBStmtThreadGroup()
{
}

void DBStmtThreadGroup::StartGroup(const char* host, u16 port, const char* user, const char* pwd, const char* db, const char* character, bool log)
{
	SetThreadGroupNum(4);
	SetThreadsName("DBStmtThread_", L"DBStmtThread_");
	bool suc = true;
	for (u32 i = 0; i < m_count; ++i)
	{
		if (m_threads[i].Init(host, port, user, pwd, db, character, false))
		{
			if (m_threads[i].LoadStmtConfig("E:/aslib/Test/mysqlTest/bin/sql.json"))
			{
				AS_LOGGER->LogEx(TIP, "DBStmtThread Load Config success, i = %u", i);
			}
			else
			{
				suc = false;
				break;
			}
		}
		else
		{
			suc = false;
			break;
		}
	}
	if (suc)
	{
		Start();
	}
}

void DBStmtThreadGroup::PrintDBInfo()
{
	u32 mc1 = 0;
	u32 mc2 = 0;
	for (u32 i = 0; i < m_count; ++i)
	{
		m_threads[i].GetMsgCount(mc1, mc2);
		AS_LOGGER->LogEx(TIP, "DBStmtThread msg1 is %u, msg2 is %u", mc1, mc2);
	}
}
