#ifndef AS_MYSQLSTMTTHREAD_H
#define AS_MYSQLSTMTTHREAD_H

#include "../thread/asMsgThread.h"
#include "asMySQLStmtQuery.h"
using namespace asNet;
class asMySQLStmtThread : public asMsgThread
{
public:
	asMySQLStmtThread();
	~asMySQLStmtThread();

	bool Init(const char* host, u16 port, const char* user, const char* pwd, const char* db, const char* character,bool log);
	bool LoadStmtConfig(const char* filePath);
private:
	// 处理网络消息
	bool OnNewNetMsg(asNetMsgHead* head, const char* data, u32 len, u32 connectID);
	// 处理非网络消息
	bool OnNewMsg(asMsg& msg);
	// insert , delete, update 无返回结果的
	bool OnNullRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID);
	// 返回一条记录
	bool OnOneRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID);
	// 返回多条记录
	bool OnMultiRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID);

	// 发送查询结果包 子类需要实现
	virtual bool SendResponse(u32 connectID, asNetMsgHead* head, char* data, u32 len) = 0;

	virtual void HandleMessage(asMsg& msg) override;
private:
	asMySQLStmtQuery	m_query;
	std::map<u32, char>	m_map;
	bool				m_log;
};

#endif
