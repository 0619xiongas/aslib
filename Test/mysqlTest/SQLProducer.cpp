#include "SQLProducer.h"
#include "buffer/asWriteBuffer.h"
#include "DBStmtThreadGroup.h"
#include "algorithm/asSnowFlake.h"
#include "time/asTime.h"
#include "utils/asStringUtils.hpp"
const char* blob_update = "This is SQLProducer Class Update";
const char* blob = "This is SQLProducer Class";

SQLProducer::SQLProducer()
{
}

SQLProducer::~SQLProducer()
{
}

void SQLProducer::DoSQLUpdate()
{
	u32 idx = m_idx % m_ids.size();
	++m_idx;
	asWriteBuffer wb;
	wb.WriteString(std::string("aslibxxx"));
	wb.WriteString(std::string("pwdxxxx"));
	wb.WriteString(std::string("email@.aslibxxxx"));
	wb.WriteI16(101);
	YMDHMS now;
	asTime::GetLocalTime(now);
	wb.WriteStruct(&now, sizeof(YMDHMS));
	wb.WriteChar(0);
	wb.WriteData(blob_update);
	asMsg msg(AS_THREADMSG_NET_RECV, 0, 0, (ulint)wb.Buf());
	DBGroup->PostMsgToMin(msg);
}

void SQLProducer::DoSQLSelect()
{

}

void SQLProducer::DoSQLDelete()
{
}

void SQLProducer::OnSQL_CB()
{
}

void SQLProducer::OnSQLUpdate_CB()
{
}

void SQLProducer::OnSQLSelect_CB()
{
}

void SQLProducer::OnSQLDelete_CB()
{
}
