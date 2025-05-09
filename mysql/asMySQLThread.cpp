#include "asMySQLThread.h"


asMySQLThread::asMySQLThread(std::function<void(asMySQLCmdParam&)> func)
	:asBaseThread(), m_conn(nullptr), m_port(0),m_addResultFunc(func),m_exit(false)
{

}

asMySQLThread::~asMySQLThread()
{
}

bool asMySQLThread::Connect2DB(const std::string& host, const std::string& user, const std::string& pwd, const std::string& db, u16 port)
{
	MYSQL* conn = mysql_init(nullptr);
	if (!conn)
	{
		return false;
	}
	mysql_options(m_conn, MYSQL_OPT_RECONNECT, "1");
	if (mysql_real_connect(conn, host.c_str(), user.c_str(), pwd.c_str(), db.c_str(), port, NULL, 0) == 0)
	{
		i32 error = mysql_errno(conn);
		//log
		return false;
	}
	if (mysql_set_character_set(conn, "utf8") != 0)
	{
		i32 error = mysql_errno(conn);
		return false;
	}
	m_conn = conn;
	m_host = host;
	m_user = user;
	m_pwd = pwd;
	m_db = db;
	m_port = port;

	return true;
}

void asMySQLThread::Close()
{
	m_exit = true;
	m_condition.notify_all();
}

void asMySQLThread::AddTask(asMySQLCmdParam& param)
{
	{
		std::lock_guard<std::mutex> lock(m_lock);
		m_tasks.emplace_back(param);
	}
	m_condition.notify_all();
}

bool asMySQLThread::Query(std::string& sql, asMySQLQueryResult* res)
{
	if (mysql_real_query(m_conn, sql.c_str(), (unsigned long)sql.length()) != 0)
	{
		return false;
	}
	// res空，取完结果集
	if (res == nullptr)
	{
		mysql_store_result(m_conn);
		while (mysql_next_result(m_conn) == 0)
		{
			mysql_store_result(m_conn);
		}
		return true;
	}
	res->AddResults(mysql_store_result(m_conn));
	while (mysql_next_result(m_conn) == 0)
	{
		res->AddResults(mysql_store_result(m_conn));
	}
	return true;
}

bool asMySQLThread::Execute(std::string& sql)
{
	return this->Query(sql, nullptr);
}

i32 asMySQLThread::HandleOneTask(asMySQLCmdParam& param)
{
	i32 ret = -1;
	switch (param.type)
	{
	case asMySQLCmdType::query:
		if (this->Query(param.sql, &(*param.res)))
			ret = 0;
		else
			ret = 1;
		break;
	case asMySQLCmdType::execute:
		if (this->Execute(param.sql))
			ret = 0;
		else
			ret = 1;
		break;
	default:
		break;
	}
	return ret;
}

void asMySQLThread::ThreadFunc()
{
	// 线程执行函数，处理m_tasks中的任务
	while (!m_exit)
	{
		std::unique_lock<std::mutex> lock(m_lock);
		if (m_tasks.size() == 0)
		{
			m_condition.wait(lock);
			continue;
		}
		i32 ret = mysql_ping(m_conn);
		if (ret)
		{
			asBaseThread::Sleep(1);
			continue;
		}
		asMySQLCmdParam param = m_tasks.front();
		m_tasks.pop_front();
		lock.unlock();

		ret = HandleOneTask(param);
		if (ret)
		{
			u32 error = mysql_errno(m_conn);
			if (error == 2013 || error == 2006 || error == 2014)
			{
				ReConnect();
				ret = HandleOneTask(param);
			}
		}
		switch (ret)
		{
		case 1:
			param.res->m_error = param.sql;
			param.res->m_error += ",";
			param.res->m_error += mysql_error(m_conn);
			param.res->m_success = false;
			if (m_addResultFunc)
				m_addResultFunc(param);
			break;
		case 0:
			if (m_addResultFunc)
				m_addResultFunc(param);
			break;
		case -1:
			break;
		default:
			break;
		}
	}
	// 需要退出，执行完剩余task
	auto itr = m_tasks.begin();
	for (;itr != m_tasks.end();++itr)
	{
		i32 r = HandleOneTask(*itr);
		switch (r)
		{
		case 1:
			itr->res->m_error = itr->sql;
			itr->res->m_error += ",";
			itr->res->m_error += mysql_error(m_conn);
			itr->res->m_success = false;
			if (m_addResultFunc)
				m_addResultFunc(*itr);
			break;
		case 0:
			if (m_addResultFunc)
				m_addResultFunc(*itr);
			break;
		case -1:
			break;
		}
	}
	if (m_conn && m_exit)
	{
		mysql_close(m_conn);
		m_conn = nullptr;
	}
}

void asMySQLThread::ReConnect()
{
	Connect2DB(m_host, m_user, m_pwd, m_db, m_port);
}
