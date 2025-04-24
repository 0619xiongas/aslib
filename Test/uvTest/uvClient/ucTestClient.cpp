#include "ucTestClient.h"
#include <iostream>
#include <string>
uvTestClient::uvTestClient()
{
}

uvTestClient::~uvTestClient()
{
}

void uvTestClient::OnNewMessage(asUvSession& session, u32 msgId, char* buf)
{
	if (buf)
	{
		std::string str(buf + sizeof(asNetTcpMsgHead));
		std::cout << "recv client " << session.GetId() << " msgID is :" << msgId << ", info : " << str << std::endl;
	}
	delete[] buf;
}

void uvTestClient::OnCloseSession(u32 sessionId)
{
}

void uvTestClient::OnAddNewSession(asUvSession& session)
{
	std::cout << "OnAddNewSession, session id : " << session.GetId() << std::endl;
	m_serverSessionId = session.GetId();
}

u32 uvTestClient::GetServerSession()
{
	return m_serverSessionId;
}
