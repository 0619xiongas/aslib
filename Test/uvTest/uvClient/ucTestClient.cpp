#include "ucTestClient.h"
#include <iostream>
#include <string>
//#include "../public/netcpp/test.pb.h"

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
		asNetTcpMsgHead* head = (asNetTcpMsgHead*)buf;
		u32 recvLen = head->m_len - sizeof(asNetTcpMsgHead);
		char* recvStr = new char[recvLen + 1];
		::memset(recvStr, 0, recvLen + 1);
		::memcpy(recvStr, buf + sizeof(asNetTcpMsgHead), recvLen);
		//ParseTestProto(recvStr, recvLen);
		recvStr[recvLen] = '\0';
		printf("server msg : %s\n", recvStr);
		delete[] recvStr;
	}
	delete[] buf;
}

void uvTestClient::OnCloseSession(u32 sessionId)
{
	std::cout << "OnDeleteSession, session id : " << sessionId << std::endl;
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

void uvTestClient::DoTestSendData()
{
	/*
	ReqBaseInfo req;
	req.set_id(123456);
	req.set_str("this is uv net work test, send to server!!!");
	u32 bufLen = req.ByteSizeLong();
	u32 totalLen = sizeof(asNetTcpMsgHead) + bufLen;
	asNetTcpMsgHead head;
	head.m_msgId = 1;
	head.m_len = totalLen;
	char* data = new char[totalLen];
	::memset(data, 0, totalLen);
	::memcpy(data, &head, sizeof(asNetTcpMsgHead));
	req.SerializePartialToArray(data + sizeof(asNetTcpMsgHead), bufLen);
	DoSendData(data, totalLen, m_serverSessionId);
	*/
	u32 sendLen = sizeof(asNetTcpMsgHead);
	asNetTcpMsgHead head;
	head.m_msgId = 1;
	std::string str = "this is uv net work test, send to server!!!";
	sendLen += str.length();
	char* data = new char[sendLen];
	head.m_len = sendLen;
	::memset(data, 0, sendLen);
	::memcpy(data, &head, sizeof(asNetTcpMsgHead));
	::memcpy(data + sizeof(asNetTcpMsgHead), str.c_str(), sendLen - sizeof(asNetTcpMsgHead));
	DoSendData(data, sendLen, m_serverSessionId);
}

void uvTestClient::ParseTestProto(char* buf, u32 len)
{
	//ResBaseInfo res;
	//if (res.ParseFromArray(buf, len))
	//{
	//}
}
