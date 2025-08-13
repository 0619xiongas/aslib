#pragma once
#include "net/asio/asAsioNetwork.h"
#include "asMsgBaseDefine.h"

class asioTestClient : public asAsioNetwork
{
public:
	asioTestClient();
	~asioTestClient();

	virtual void OnNewMessage(asAsioSession& session, u32 msgId, char* buf) override;

	virtual void OnCloseSession(u32 sessionId) override;

	virtual void OnAddNewSession(asAsioSession& session) override;

	u32 GetServerSession();

	void DoTestSendData();

	void ParseTestProto(char* buf, u32 len);
private:
	u32 m_serverSessionId = 0;
};

