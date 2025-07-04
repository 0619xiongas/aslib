#include "../../../include/mysql/primary/asMySQLThread.h"


asMySQLThread::asMySQLThread(std::function<void(asMySQLCmdParam&)> func)
	:asWorkThread(),m_addResultFunc(func),m_exit(false)
{

}

asMySQLThread::~asMySQLThread()
{
}

bool asMySQLThread::Init(const char* host, const char* user, const char* pwd, const char* db, u16 port, const char* character)
{
	return m_mysql.Init(host,port,user,pwd,db,character);
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
	if (mysql_real_query(m_mysql.m_conn, sql.c_str(), (unsigned long)sql.length()) != 0)
	{
		return false;
	}
	// res空，取完结果集
	if (res == nullptr)
	{
		mysql_store_result(m_mysql.m_conn);
		while (mysql_next_result(m_mysql.m_conn) == 0)
		{
			mysql_store_result(m_mysql.m_conn);
		}
		return true;
	}
	res->AddResults(mysql_store_result(m_mysql.m_conn));
	while (mysql_next_result(m_mysql.m_conn) == 0)
	{
		res->AddResults(mysql_store_result(m_mysql.m_conn));
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
		i32 ret = mysql_ping(m_mysql.m_conn);
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
			u32 error = mysql_errno(m_mysql.m_conn);
			if (error == 2013 || error == 2006 || error == 2014)
			{
				m_mysql.ReConnect();
				ret = HandleOneTask(param);
			}
		}
		switch (ret)
		{
		case 1:
			param.res->m_error = param.sql;
			param.res->m_error += ",";
			param.res->m_error += mysql_error(m_mysql.m_conn);
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
	if (m_tasks.size())
	{
		auto itr = m_tasks.begin();
		for (;itr != m_tasks.end();++itr)
		{
			i32 r = HandleOneTask(*itr);
			switch (r)
			{
			case 1:
				itr->res->m_error = itr->sql;
				itr->res->m_error += ",";
				itr->res->m_error += mysql_error(m_mysql.m_conn);
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
	}
	if (m_exit)
	{
		m_mysql.Close();
	}
}
