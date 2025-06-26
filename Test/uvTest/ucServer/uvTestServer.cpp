#include "uvTestServer.h"
#include <iostream>
#include "../public/netcpp/test.pb.h"

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
		asNetTcpMsgHead* head = (asNetTcpMsgHead*)buf;
		u32 recvLen = head->m_len - sizeof(asNetTcpMsgHead);
		char* recvStr = new char[recvLen];
		::memset(recvStr, 0, recvLen);
		::memcpy(recvStr, buf + sizeof(asNetTcpMsgHead), recvLen);
		ParseTestProto(recvStr, recvLen);
		//recvStr[recvLen] = '\0';
		//printf("client msg : %s\n", recvStr);
		delete[] recvStr;
	}
	
	ResBaseInfo res;
	res.set_ret(0);
	res.set_id(123456);
	res.set_str("this is uv net work test,send to client!!!");
	u32 bufLen = res.ByteSizeLong();
	u32 totalLen = sizeof(asNetTcpMsgHead) + bufLen;
	asNetTcpMsgHead head;
	head.m_msgId = 1;
	head.m_len = totalLen;
	char* data = new char[totalLen];
	::memset(data, 0, totalLen);
	::memcpy(data, &head, sizeof(asNetTcpMsgHead));
	res.SerializePartialToArray(data + sizeof(asNetTcpMsgHead),bufLen);
	DoSendData(data, totalLen, session.GetId());

	//u32 sendLen = sizeof(asNetTcpMsgHead);
	//asNetTcpMsgHead head;
	//head.m_msgId = 10000;
	//std::string str = "this is uv net work test,send to client!!!";
	//sendLen += str.length();
	//char* data = new char[sendLen];
	//head.m_len = sendLen;
	//::memset(data, 0, sendLen);
	//::memcpy(data, &head, sizeof(asNetTcpMsgHead));
	//::memcpy(data + sizeof(asNetTcpMsgHead), str.c_str(), str.length());
	//DoSendData(data, sendLen, session.GetId());

	delete[] buf;
}

void uvTestServer::OnCloseSession(u32 sessionId)
{
	std::cout << "OnDeleteSession, session id : " << sessionId << std::endl;
}

void uvTestServer::OnAddNewSession(asUvSession& session)
{
	std::cout << "OnAddNewSession, session id : " << session.GetId() << std::endl;
}

void uvTestServer::ParseTestProto(char* buf, u32 len)
{
	ReqBaseInfo req;
	if (req.ParseFromArray(buf, len))
	{
		std::cout << "msg str : " << req.str() << std::endl;
	}
}
