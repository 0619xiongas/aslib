#pragma once
#include "net/uv/asUvNetWork.h"
#include "asMsgBaseDefine.h"

class uvTestServer : public asUvNetWork
{
public:
	uvTestServer();
	~uvTestServer();

	virtual void OnNewMessage(asUvSession& session, u32 msgId, char* buf) override;

	virtual void OnCloseSession(u32 sessionId) override;

	virtual void OnAddNewSession(asUvSession& session) override;

	void ParseTestProto(char* buf,u32 len);
};