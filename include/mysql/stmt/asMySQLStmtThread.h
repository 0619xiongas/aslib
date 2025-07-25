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
	// ����������Ϣ
	bool OnNewNetMsg(asNetMsgHead* head, const char* data, u32 len, u32 connectID);
	// �����������Ϣ
	bool OnNewMsg(asMsg& msg);
	// insert , delete, update �޷��ؽ����
	bool OnNullRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID);
	// ����һ����¼
	bool OnOneRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID);
	// ���ض�����¼
	bool OnMultiRecord(u32 id, asNetMsgHead* head, const char* data, u32 len, u32 connectID);

	// ���Ͳ�ѯ����� ������Ҫʵ��
	virtual bool SendResponse(u32 connectID, asNetMsgHead* head, char* data, u32 len) = 0;

	virtual void HandleMessage(asMsg& msg) override;
private:
	asMySQLStmtQuery	m_query;
	std::map<u32, char>	m_map;
	bool				m_log;
};

#endif
