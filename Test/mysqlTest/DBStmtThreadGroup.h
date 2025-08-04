#pragma once
#include "thread/asMsgThreadGroup.h"
#include "DBStmtThread.h"
#include "single/asSingle.h"

class DBStmtThreadGroup : public asMsgThreadGroup<DBStmtThread>
{
public:
	DBStmtThreadGroup();
	~DBStmtThreadGroup();

	void StartGroup(const char* host, u16 port, const char* user, const char* pwd, const char* db, const char* character, bool log);

	void PrintDBInfo();
};

#define	DBGroup asSingleton<DBStmtThreadGroup>::instance()