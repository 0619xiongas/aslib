#pragma once
#include "mysql/stmt/asMySQLStmtThread.h"

class DBStmtThread : public asMySQLStmtThread
{
public:
	DBStmtThread();

	~DBStmtThread();

	virtual bool SendResponse(u32 connectID, asNetMsgHead* head, char* data, u32 len) override;

	virtual bool LoadStmtConfig(const char* filePath) override;
};