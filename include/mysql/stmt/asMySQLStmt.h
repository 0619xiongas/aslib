#ifndef AS_MYSQLSTMT_H
#define AS_MYSQLSTMT_H

#include "../asMySQLDefine.h"
#include <string>
class asMySQLStmt
{
public:
    asMySQLStmt();
    ~asMySQLStmt();

private:
    MYSQL_STMT* m_stmt;
    MYSQL_BIND* m_inBinds;
    MYSQL_BIND* m_outBinds;
    u32                 m_inCount;
    u32                 m_outCount;
    std::string         m_sql;

};
#endif