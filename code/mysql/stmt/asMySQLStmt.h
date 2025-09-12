#ifndef AS_MYSQLSTMT_H
#define AS_MYSQLSTMT_H

#include "../asMySQLDefine.h"
#include <string>
#include <stdarg.h>
/*
    负责管理一条预编译sql语句
*/
class asMySQLStmtQuery;
class asMySQLStmt
{
public:
    explicit asMySQLStmt(asMySQLStmtQuery* conn);
    ~asMySQLStmt();
    // 清理资源
    void Clear();
    // 关闭预编译
    void Close();
    // 解析参数 绑定参数
    i32 InitStmtParams(const char* in,const char* out,const char* sql,const char flag);
    // 参数绑定
    i32 PrepareStmt();
    //执行语句
    i32 ExecuteQuery(u32 num, ...);
    i32 ExecuteQuery(u32 num, va_list& args);
    i32 ExecuteQuery(u32 len, const char* data);
    // 获取结果类型
    u8 GetQueryType();
    // 清理结果集
    void ClearResult();
    // 获取结果集 写入buffer
    i32 GetResultsToBuffer();
private:
    void ResetBindData();

    i32 Execute(MYSQL_BIND* binds);
private:
    asMySQLStmtQuery*   m_query;
    MYSQL_STMT*         m_stmt;
    MYSQL_BIND*         m_inBinds;
    MYSQL_BIND*         m_outBinds;
    u8                  m_resFlag;
    u32                 m_inCount;
    u32                 m_outCount;
    asStmtParam**       m_inParams;
    asStmtParam**       m_outParams;
    std::string         m_sql;

};
#endif