#ifndef AS_MYSQLSTMTTHREAD_H
#define AS_MYSQLSTMTTHREAD_H

#include "../../thread/asMsgThread.h"
#include "asMySQLStmtQuery.h"
using namespace asNet;
class asMySQLStmtThread : public asMsgThread
{
public:
	asMySQLStmtThread();
	~asMySQLStmtThread();

	bool Init(const char* host, u16 port, const char* user, const char* pwd, const char* db, const char* character,bool log);
	bool LoadStmtConfig(u32 id, const char* in, const char* out, const char* sql, const char flag);

protected:
	virtual bool LoadStmtConfig(const char* filePath);
	virtual bool OnNewMsg(asMsg& msg);
	virtual bool SendResponse(u32 connectID, asNetMsgHead* head, char* data, u32 len) = 0;
private:

	bool OnNewNetMsg(asNetMsgHead* head, const char* data, u32 len, u32 connectID);

	bool OnNullRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID);

	bool OnOneRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID);

	bool OnMultiRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID);

	virtual void HandleMessage(asMsg& msg) override;
protected:
	asMySQLStmtQuery	m_query;
	std::map<u32, char>	m_map;
	bool				m_log;
};

#endif
