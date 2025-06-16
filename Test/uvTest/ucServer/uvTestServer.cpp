#include "uvTestServer.h"
#include <iostream>
uvTestServer::uvTestServer()
{
}

uvTestServer::~uvTestServer()
{
}

void uvTestServer::OnNewMessage(asUvSession& session, u32 msgId, char* buf)
{
	if (buf)
	{
		std::string str(buf+sizeof(asNetTcpMsgHead));
		std::cout<< "recv client "<< session.GetId()<< " msgID is :" << msgId << ", info : " << str << std::endl;
	}
	std::string info  = "this is uv net work test,send to client!!!";
	u32 sendLen = sizeof(asNetTcpMsgHead);
	sendLen += info.length();
	asNetTcpMsgHead head;
	head.m_msgId = msgId + 0x10000000;
	head.m_len = sendLen;
	char* data = new char[sendLen];
	::memcpy(data, &head, sizeof(asNetTcpMsgHead));
	::memcpy(data + sizeof(asNetTcpMsgHead), (char*)info.c_str(), info.length());
	DoSendData(data, sendLen, session.GetId());
	delete[] buf;
}

void uvTestServer::OnCloseSession(u32 sessionId)
{
	std::cout << "OnAddNewSession, session id : " << sessionId << std::endl;
}

void uvTestServer::OnAddNewSession(asUvSession& session)
{
	std::cout << "OnAddNewSession, session id : " << session.GetId() << std::endl;
}
