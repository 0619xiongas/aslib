#ifndef AS_MYSQLCONNECTION_H
#define AS_MYSQLCONNECTION_H

#include "asMySQLDefine.h"

/*
* 封装MYSQL的连接,封装基础api，作为组件。结果集不在此
*/
class asMySQLThread;
class asMySQLQueryMgr;
class asMySQLStmt;

class asMySQLConnection final
{
	friend class asMySQLThread;
	friend class asMySQLQueryMgr;
	friend class asMySQLStmt;
public:
	asMySQLConnection();
	~asMySQLConnection();

	bool Init(const char* host, u16 port, const char* user, const char* pwd, const char* db, const char* character);

	void Close();

	void SetCharacter(const char* option);

	void SelectDB(const char* db);

	bool Transaction();

	bool Commit();

	bool Rollback();

	bool Connect();

	bool ReConnect();

	const char* GetErrorString();

	u32 GetErrorCode();
private:
	MYSQL*			m_conn;
	std::string		m_host;
	std::string		m_user;
	std::string		m_pwd;
	std::string		m_db;
	std::string		m_char;
	u16				m_port;
};

#endif