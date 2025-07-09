#ifndef AS_MYSQLSTMTQUERY_H
#define AS_MYSQLSTMTQUERY_H

#include "asMySQLStmt.h"
#include "../asMySQLConnection.h"
#include <map>
#include "../../buffer/asNodeBuffer.h"

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
	asNodeBuffer		m_results;
	u32					m_rows;
	u32					m_fields;
	u32					m_affectCount;
	u64					m_insertId;
};

#endif