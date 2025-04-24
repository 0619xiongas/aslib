#pragma once
#include "../aslib/net/uv/asUvNetWork.h"

class uvTestClient : public asUvNetWork
{
public:
	uvTestClient();
	~uvTestClient();

	virtual void OnNewMessage(asUvSession& session, u32 msgId, char* buf) override;

	virtual void OnCloseSession(u32 sessionId) override;

	virtual void OnAddNewSession(asUvSession& session) override;

	u32 GetServerSession();
private:
	u32 m_serverSessionId = 0;
};

