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
	// 加载sql配置文件
	bool LoadMySQLStmtMap(const char* path);
	// 自定义参数绑定(进map)
	i32 PrepareStmtParams(u32 id,const char* in, const char* out, const char* sql,const char flag);
	i32 PrepareStmtParms(u32 id, const char* stmtStr, const char flag);
	// 执行
	i32 Query(const char* in, const char* out, const char* sql, const char flag,i32 num,...);
	i32 Query(const char* in, const char* out, const char* sql, const char flag, u32 len, const char* data);
	i32 QueryByID(u32 id, i32 num, ...);
	i32 QueryByID(u32 id, u32 len, const char* data);

	//获取结果集
	asNodeBuffer GetResults() { return m_results; }
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