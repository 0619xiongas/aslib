#ifndef AS_MYSQLSTMTQUERY_H
#define AS_MYSQLSTMTQUERY_H

#include "asMySQLStmt.h"
#include "../asMySQLConnection.h"
#include <map>
#include "../../buffer/asNodeBuffer.h"

class asMySQLStmtThread;
class asReadBuffer;
class asMySQLStmtQuery
{
	friend class asMySQLStmt;
	friend class asMySQLStmtThread;
public:
	asMySQLStmtQuery();
	~asMySQLStmtQuery();
	// 自定义参数绑定(进map)
	/*
	* stmtStr格式
	* @{传入参数类型列表}
	* #{输入参数类型列表}
	* ${stmt sql 语句}
	* "@{i32}#{i16,i32,u32}${select `age`,`money`,`score` from user where `id` = ?;}"
	*/
	i32 PrepareStmtParams(u32 id, const char* stmtStr, const char flag);
	i32 PrepareStmtParams(u32 id,const char* in, const char* out, const char* sql,const char flag);
	// 执行 (用m_stmt)
	i32 Query(const char* in, const char* out, const char* sql, const char flag,i32 num,...);
	i32 Query(const char* in, const char* out, const char* sql, const char flag, u32 len, const char* data);
	// map 集合中找
	i32 QueryByID(u32 id, i32 num, ...);
	i32 QueryByID(u32 id, u32 len, const char* data);

	//获取结果集
	asNodeBuffer& GetResults() { return m_results; }
	void GetOneResult(asReadBuffer& rb);
	//获取类型
	u8 GetQueryType(u32 id);
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