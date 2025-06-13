#ifndef AS_MYSQLDEFINE_H
#define AS_MYSQLDEFINE_H

#include "../asBaseDefine.h"
#ifdef _WIN32
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif

enum class asMySQLCmdType
{
	query,
	execute,
};

#endif