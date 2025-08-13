#pragma once
#include "net/asio/asAsioNetwork.h"
#include "asMsgBaseDefine.h"

class asioTestServer : public asAsioNetwork
{
public:
	asioTestServer();
	~asioTestServer();

	virtual void OnNewMessage(asAsioSession& session, u32 msgId, char* buf) override;

	virtual void OnCloseSession(u32 sessionId) override;

	virtual void OnAddNewSession(asAsioSession& session) override;

	void ParseTestProto(char* buf,u32 len);
};