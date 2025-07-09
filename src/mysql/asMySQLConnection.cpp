#include "../../../include/mysql/asMySQLConnection.h"

asMySQLConnection::asMySQLConnection():m_conn(NULL),m_port(0)
{
}

asMySQLConnection::~asMySQLConnection()
{
	Close();
}

bool asMySQLConnection::Init(const char* host, u16 port, const char* user, const char* pwd, const char* db, const char* character)
{
	m_host = host;
	m_port = port;
	m_user = user;
	m_pwd = pwd;
	m_db = db;
	m_char = character;
	return Connect();

}

void asMySQLConnection::Close()
{
	if (m_conn)
	{
		while (mysql_next_result(m_conn) == 0)
		{
			MYSQL_RES* res = mysql_store_result(m_conn);
			mysql_free_result(res);
		}
	}
	mysql_close(m_conn);
	m_conn = NULL;
}

void asMySQLConnection::SetCharacter(const char* option)
{
	m_char = option;
	if (m_conn)
	{
		mysql_set_character_set(m_conn, m_char.c_str());
	}
}

void asMySQLConnection::SelectDB(const char* db)
{
	if (ReConnect())
	{
		if (mysql_select_db(m_conn, db) == 0)
		{
			m_db = db;
		}
		else
		{
			// log
		}
	}
}

bool asMySQLConnection::Transaction()
{
	if (ReConnect())
	{
		if (mysql_real_query(m_conn, "BEGIN;",6))
		{
			return false;
		}
		return true;
	}
	return false;
}

bool asMySQLConnection::Commit()
{
	if (ReConnect())
	{
		if (mysql_real_query(m_conn, "COMMIT;", 7))
		{
			return false;
		}
		return true;
	}
	return false;
}

bool asMySQLConnection::Rollback()
{
	if (ReConnect())
	{
		if (mysql_real_query(m_conn, "ROLLBACK;", 9))
		{
			return false;
		}
		return true;
	}
	return false;
}

bool asMySQLConnection::Connect()
{
	if (m_conn)
	{
		return true;
	}
	else
	{
		m_conn = mysql_init(NULL);
		if (!m_conn)
		{
			// log
			return false;
		}
		my_bool reconn = 1;
		mysql_options(m_conn, MYSQL_OPT_RECONNECT, &reconn);
		if (!mysql_real_connect(m_conn, m_host.c_str(), m_user.c_str(), m_pwd.c_str(), m_db.c_str(), m_port, NULL, CLIENT_MULTI_STATEMENTS | CLIENT_INTERACTIVE))
		{
			// log
			mysql_close(m_conn);
			m_conn = NULL;
			return false;
		}
		if (mysql_set_character_set(m_conn, m_char.c_str()) != 0)
		{
			mysql_close(m_conn);
			m_conn = NULL;
			return false;
		}
		return true;
	}
	return false;
}

bool asMySQLConnection::ReConnect()
{
	if (m_conn)
	{
		if (mysql_ping(m_conn))
		{
			Close();
			return Connect();
		}
		return true;
	}
	else
	{
		return Connect();
	}
	return false;
}

const char* asMySQLConnection::GetErrorString()
{
	return mysql_error(m_conn);
}

u32 asMySQLConnection::GetErrorCode()
{
	return mysql_errno(m_conn);
}
