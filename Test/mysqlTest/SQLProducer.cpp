#include "SQLProducer.h"
#include "buffer/asWriteBuffer.h"
#include "DBStmtThreadGroup.h"
#include "time/asTime.h"
#include "tools/asStringUtils.hpp"
#include <iostream>
#include "buffer/asNodeBuffer.h"
#include "buffer/asReadBuffer.h"
const char* g_blob_update = "This is SQLProducer Class Update\0";
const char* g_blob = "This is SQLProducer Class\0";

SQLProducer::SQLProducer()
{
	m_sf.Init(1000);
}

SQLProducer::~SQLProducer()
{
}

void SQLProducer::DoSQLUpdate()
{
	u32 idx = m_idx % m_ids.size();
	i64 id = m_ids[m_idx];
	++m_idx;
	//create net data
	asWriteBuffer wb;
	wb.Set(256);
	wb.WriteString("aslibxxx");
	wb.WriteString("pwdxxxx");
	wb.WriteString("email@.aslibxxxx");
	wb.WriteI16(101);
	YMDHMS now;
	asTime::GetLocalTime(now);
	wb.WriteData(&now, sizeof(YMDHMS));
	wb.WriteChar(0);
	wb.WriteString(g_blob_update,strlen(g_blob_update));
	wb.WriteI64(id);

	u32 sendLen = sizeof(asNetMsgHead) + wb.Size();
	asNetMsgHead head;
	head.m_msgId = 0x10000003;
	head.m_len = sendLen;
	char* buf = new char[sendLen];

	memcpy(buf, &head, sizeof(asNetMsgHead));
	memcpy(buf + sizeof(asNetMsgHead), wb.Buf(), wb.Size());

	asMsg msg(AS_THREADMSG_NET_RECV, 0x10000003, 0, (ulint)buf);
	DBGroup->PostMsgToMin(msg);
	std::cout << "SQLProducer::DoSQLUpdate, id is " << id << std::endl;
}

void SQLProducer::DoSQLSelect()
{
	asNetMsgHead head;
	head.m_msgId = 0x10000002;
	head.m_len = sizeof(asNetMsgHead);
	char* buf = new char[head.m_len];
	memcpy(buf, &head, sizeof(asNetMsgHead));

	asMsg msg(AS_THREADMSG_NET_RECV, 0x10000002, 0, (ulint)buf);
	DBGroup->PostMsgToMin(msg);
}

void SQLProducer::DoSQLDelete()
{
	i32 idx = m_idxRandom % m_ids.size();
	i64 id = m_ids[idx];
	++m_idxRandom;
	u32 len = sizeof(asNetMsgHead) + sizeof(i64);
	char* buf = new char[len];
	asNetMsgHead head;
	head.m_msgId = 0x10000004;
	head.m_len = len;

	memcpy(buf, &head, sizeof(asNetMsgHead));
	memcpy(buf + sizeof(asNetMsgHead), &id, sizeof(i64));
	asMsg msg(AS_THREADMSG_NET_RECV, 0x10000004, 0, (ulint)buf);
	DBGroup->PostMsgToMin(msg);

	std::cout << "SQLProducer::DoSQLDelete, id is " << id << std::endl;
}

void SQLProducer::DoSQLInsert()
{
	i64 id = m_sf.GetId();
	m_ids.push_back(id);

	asWriteBuffer wb;
	wb.Set(256);
	wb.WriteI64(id);
	wb.WriteString("aslib");
	wb.WriteString("pwd");
	wb.WriteString("email@.aslib");
	wb.WriteI16(11);
	YMDHMS now;
	asTime::GetLocalTime(now);
	wb.WriteData(&now, sizeof(YMDHMS));
	wb.WriteChar(1);
	wb.WriteString(g_blob,strlen(g_blob));

	u32 sendLen = sizeof(asNetMsgHead) + wb.Size();
	asNetMsgHead head;
	head.m_msgId = 0x10000001;
	head.m_len = sendLen;
	char* buf = new char[sendLen];

	memcpy(buf, &head, sizeof(asNetMsgHead));
	memcpy(buf + sizeof(asNetMsgHead), wb.Buf(), wb.Size());

	asMsg msg(AS_THREADMSG_NET_RECV, 0x10000001, 0, (ulint)buf);
	DBGroup->PostMsgToMin(msg);

	std::cout << "SQLProducer::DoSQLInsert, id is " << id << std::endl;
}

void SQLProducer::OnSQL_CB(asNetMsgHead* head, char* data, u32 len)
{
	if (head->m_msgId == 0x10000002)
	{
		OnSQLSelect_CB(head, data, len);
	}
}

void SQLProducer::OnSQLSelect_CB(asNetMsgHead* head, char* data, u32 len)
{
	asNodeBuffer nb;
	data += AS_MSG_HEAD_SIZE;
	asDBNetParam* param = (asDBNetParam*)data;
	if (param->rows > 0)
	{
		asReadBuffer rb;
		nb.FormatFrom(data + sizeof(asDBNetParam), len - AS_MSG_HEAD_SIZE - sizeof(asDBNetParam));
		auto itr = nb.Begin();
		while (itr.IsInvaild())
		{
			rb.Set(itr.Data(), itr.Size());
			i64 id;
			std::string name;
			std::string password;
			std::string email;
			i16 age;
			YMDHMS dt;
			char gender;
			std::string info;
			rb >> id >> name >> password >> email >> age;
			rb.ReadData(&dt, sizeof(YMDHMS));
			char timeStr[TIME_STR_MAX_LEN];
			dt.FormatTime(timeStr);
			rb >> gender >> info;
			std::cout << "SQLProducer::OnSQLSelect_CB, id: " << id
				<< ", name: " << name
				<< ", password: " << password
				<< ", email: " << email
				<< ", age: " << age
				<< ", birth: " << timeStr
				<< ", gender: " << gender
				<< ", info: " << info << std::endl;

			nb.Next(itr);
		}
	}
}

