#ifndef AS_MYSQLSTMTQUERY_H
#define AS_MYSQLSTMTQUERY_H

#include "asMySQLStmt.h"
#include "../asMySQLConnection.h"
#include <map>

class asMySQLStmtQuery
{
	friend class asMySQLStmt;
public:
	asMySQLStmtQuery();
	~asMySQLStmtQuery();
	// 加载sql配置文件，使用整个功能前必须调用此函数
	bool LoadMySQLStmtMap(const char* path);


private:
	asMySQLConnection	m_mysql;
	asMySQLStmt			m_stmt;
	std::map<u32, asMySQLStmt> m_stmts;
};

#endif