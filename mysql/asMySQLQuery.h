#ifndef AS_SYNCMYSQLQUERY_H
#define AS_SYNCMYSQLQUERY_h

/*
*	实现一个同步查询数据库数据的类，执行mysql_query会阻塞当前线程
*/
#include"asMySQLThread.h"
#include <vector>
/*
* 连接管理，管理几个线程连接数据库
*/
class asMySQLQueryMgr
{
public:
	asMySQLQueryMgr();
	~asMySQLQueryMgr();

	// 初始化
	bool Init(const std::string& host, const std::string& user, const std::string& pwd,
		const std::string& db, u16 port,u32 threadCount = 4);
	void Close();

	// 处理结果
	void Update();

	// 同步执行 阻塞当前线程
	bool SyncExecute(const char* sql);
	bool SyncQuery(const char* sql, asMySQLQueryResult& res);
	bool SyncQuery(const char* sql, asMySQLQueryResult* pRes);

	// 异步
	void AsyncExecute(u32 rand,const char* sql, MySQLResultCB& cb,void *userdata = nullptr);
	void AsyncQuery(u32 rand,const char* sql, MySQLResultCB& cb,void* userdata = nullptr);

	void AddResult(asMySQLCmdParam& param);
private:
	MYSQL* m_conn;
	std::mutex	m_mutex;
	std::vector<asMySQLThread* > m_threads;
	std::vector<asMySQLCmdParam> m_results; // 各线程执行完的结果， 
	u32	m_threadCount;
};

#endif
