#pragma once
#include "net/uv/asUvNetWork.h"
#include "asMsgBaseDefine.h"

class uvTestClient : public asUvNetWork
{
public:
	uvTestClient();
	~uvTestClient();

	virtual void OnNewMessage(asUvSession& session, u32 msgId, char* buf) override;

	virtual void OnCloseSession(u32 sessionId) override;

	virtual void OnAddNewSession(asUvSession& session) override;

	u32 GetServerSession();

	void DoTestSendData();

	void ParseTestProto(char* buf, u32 len);
private:
	u32 m_serverSessionId = 0;
};

