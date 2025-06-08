#ifndef AS_MYSQLTHREAD_H
#define AS_MYSQLTHREAD_H 

#include "../thread/asWorkThread.h"
#include "asMySQLQueryResult.h"
#include <list>
#include <mutex>
#include <condition_variable> 
#ifndef _WIN32
#define my_bool bool
#endif

class asMySQLQueryMgr;
/*
* 实现一个数据库执行语句线程
*/
class asMySQLThread : public asWorkThread
{
	friend class asMySQLQueryMgr;
public:
	explicit asMySQLThread(std::function<void(asMySQLCmdParam&)> func);
	~asMySQLThread();

	bool Connect2DB(const std::string& host, const std::string& user, const std::string& pwd,
		const std::string& db, u16 port);

	// 关闭数据库资源的连接 线程资源交给basethread
	void Close();
	
	//添加一个sql命令参数集
	void AddTask(asMySQLCmdParam& param);
private:
	bool Query(std::string& sql, asMySQLQueryResult* res);
	bool Execute(std::string& sql);
	i32 HandleOneTask(asMySQLCmdParam& param);
protected:
	virtual void ThreadFunc() override;

private:
	void ReConnect();
private:
	MYSQL* m_conn;
	std::list<asMySQLCmdParam> m_tasks;
	std::mutex m_lock;
	std::condition_variable m_condition;
	std::function<void(asMySQLCmdParam&)> m_addResultFunc;
	std::string	m_host;
	std::string	m_user;
	std::string	m_pwd;
	std::string	m_db;
	u16	m_port;
	bool m_exit;
};

#endif // !AS_MYSQLTHREAD_H

