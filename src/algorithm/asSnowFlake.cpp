#include "../../include/algorithm/asSnowflake.h"

asSnowFlake::asSnowFlake()
	:m_curTime(0),m_timeBegin(AS_SNOW_DEFAULT_TIMEGEBIN),m_seqId(0),m_serverId(0)
{

}
asSnowFlake::~asSnowFlake()
{
}

bool asSnowFlake::Init(u16 serverId, time_t timeBegin)
{
	if (serverId >= (1 << 12))
		return false;
	m_serverId = serverId;
	m_timeBegin = timeBegin;
	return true;
}

i64 asSnowFlake::GetId()
{
	i64 id = 0;
	m_lock.lock();
	do
	{
		time_t t = ::time(0) - m_timeBegin;
		if (t != m_curTime)
		{
			m_curTime = t;
			m_seqId = 0;
		}
		id = static_cast<i64>(m_curTime << 32);
		id |= static_cast<i64>(m_serverId << 20);
		id |= m_seqId;
		++m_seqId;
	} while (m_seqId >= AS_SNOW_SEQ_MAX);
	m_lock.unlock();
	return id;
}

u16 asSnowFlake::GetServerId(i64 id)
{
	return (u16)((id >> 20) & 0xFFF);
}

i32 asSnowFlake::GetTimeStamp(i64 id)
{
	return (i32)(id >> 32);
}

u32 asSnowFlake::GetSeqId(i64 id)
{
	return (u32)(id & 0xFFFFF);
}
