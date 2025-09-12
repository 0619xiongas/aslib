#ifndef AS_MYSQLTHREAD_H
#define AS_MYSQLTHREAD_H 

#include "../../thread/asBaseThread.h"
#include "../asMySQLConnection.h"
#include "asMySQLQueryResult.h"
#include <list>
#include <mutex>
#include <condition_variable>

class asMySQLQueryMgr;
/*
* 实现一个数据库执行语句线程
*/
class asMySQLThread : public asBaseThread
{
	friend class asMySQLQueryMgr;
public:
	explicit asMySQLThread(std::function<void(asMySQLCmdParam&)> func);
	~asMySQLThread();

	bool Init(const char* host, const char* user, const char* pwd,const char* db, u16 port,const char* character);
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
	asMySQLConnection	m_mysql;
	std::list<asMySQLCmdParam> m_tasks;
	std::mutex m_lock;
	std::condition_variable m_condition;
	std::function<void(asMySQLCmdParam&)> m_addResultFunc;
};

#endif // !AS_MYSQLTHREAD_H

