#include "../../../include/mysql/primary/asMySQLQuery.h"
asMySQLQueryMgr::asMySQLQueryMgr()
	:m_threadCount(0),m_conn(nullptr),m_close(false)
{
}

asMySQLQueryMgr::~asMySQLQueryMgr()
{
	if (!m_close)
	{
		this->Close();
	}
}

bool asMySQLQueryMgr::Init(const std::string& host, const std::string& user, const std::string& pwd, const std::string& db, u16 port, u32 threadCount)
{
	if (m_conn)
	{
		mysql_close(m_conn);
		m_conn = nullptr;
	}
	m_conn = mysql_init(NULL);
	if (mysql_real_connect(m_conn, host.c_str(), user.c_str(), pwd.c_str(), db.c_str(), port, NULL, 0) == 0)
	{
		i32 error = mysql_errno(m_conn);
		//log
		return false;
	}
	if (mysql_set_character_set(m_conn, "utf8") != 0)
	{
		i32 error = mysql_errno(m_conn);
		return false;
	}
	if (m_threads.empty())
	{
		m_threadCount = threadCount;
		m_threads.reserve(m_threadCount);
		for (size_t i = 0; i < m_threadCount;++i)
		{
			asMySQLThread* thread = new asMySQLThread(std::bind(&asMySQLQueryMgr::AddResult, this, std::placeholders::_1));
			if (!thread || !thread->StartThread())
			{
				delete thread;
				thread = nullptr;
				continue;
			}
			if (!thread->Connect2DB(host, user, pwd, db, port))
			{
				delete thread;
				thread = nullptr;
			}
			std::wstring name = L"mysql thread";
			name += std::to_wstring(i);
			thread->SetThreadName("", name.c_str());
			m_threads.push_back(thread);
		}
		if (m_threads.empty())
		{
			//log and print .... 
			return false;
		}
	}
	return true;
}

void asMySQLQueryMgr::Close()
{
	m_close = true;
	if (m_conn)
	{
		mysql_close(m_conn);
		m_conn = nullptr;
	}
	for (size_t i = 0;i < m_threads.size();++i)
	{
		m_threads[i]->Close();
	}
	for (size_t i = 0;i < m_threads.size();++i)
	{
		delete m_threads[i];
		m_threads[i] = nullptr;
	}
	
}

void asMySQLQueryMgr::Update()
{
	std::vector<asMySQLCmdParam> cache;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		cache = m_results;
		m_results.clear();
	}
	for (size_t i = 0;i < cache.size();++i)
	{
		if (cache[i].cb)
		{
			cache[i].cb(*cache[i].res);
		}
	}
}

bool asMySQLQueryMgr::SyncExecute(const char* sql)
{
	return SyncQuery(sql,nullptr);
}

bool asMySQLQueryMgr::SyncQuery(const char* sql, asMySQLQueryResult& res)
{
	return SyncQuery(sql, &res);
}

bool asMySQLQueryMgr::SyncQuery(const char* sql, asMySQLQueryResult* pRes)
{
	if (mysql_real_query(m_conn, sql, (unsigned long)strlen(sql)) != 0)
	{
		if (pRes)
		{
			pRes->m_success = false;
			pRes->m_error = sql;
			pRes->m_error += ",";
			pRes->m_error += mysql_errno(m_conn);
		}
		return false;
	}
	// res空，取完结果集
	if (pRes == nullptr)
	{
		mysql_store_result(m_conn);
		while (mysql_next_result(m_conn) == 0)
		{
			mysql_store_result(m_conn);
		}
		return true;
	}
	pRes->AddResults(mysql_store_result(m_conn));
	while (mysql_next_result(m_conn) == 0)
	{
		pRes->AddResults(mysql_store_result(m_conn));
	}
	return true;
}

void asMySQLQueryMgr::AsyncExecute(u32 rand,const char* sql, MySQLResultCB& cb)
{
	asMySQLCmdParam param;
	param.cb = cb;
	param.sql = sql;
	param.type = asMySQLCmdType::execute;
	param.res = std::shared_ptr<asMySQLQueryResult>(new asMySQLQueryResult);
	m_threads[rand % m_threadCount]->AddTask(param);
}

void asMySQLQueryMgr::AsyncQuery(u32 rand,const char* sql, MySQLResultCB& cb)
{
	asMySQLCmdParam param;
	param.cb = cb;
	param.sql = sql;
	param.type = asMySQLCmdType::query;
	param.res = std::shared_ptr<asMySQLQueryResult>(new asMySQLQueryResult);
	m_threads[rand % m_threadCount]->AddTask(param);
}

void asMySQLQueryMgr::AddResult(asMySQLCmdParam& param)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_results.emplace_back(param);
}
