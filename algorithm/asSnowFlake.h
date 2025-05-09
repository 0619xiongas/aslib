#ifndef AS_SNOWFLAKE_H
#define AS_SNOWFLAKE_H

#include <time.h>
#include <mutex>
#include "../asBaseDefine.h"

#define AS_SNOW_DEFAULT_TIMEGEBIN 1742954400 //2025-3-26 10:00:00
#define AS_SNOW_SEQ_MAX 1048576	

class asSnowFlake
{
public:
	asSnowFlake();
	~asSnowFlake();

	bool Init(u16 serverId, time_t timeBegin = AS_SNOW_DEFAULT_TIMEGEBIN);

	i64 GetId();

	u16 GetServerId(i64 id);
	i32 GetTimeStamp(i64 id);
	u32 GetSeqId(i64 id);

public:
	time_t		m_curTime;
	time_t		m_timeBegin;
	u32			m_seqId;
	u16			m_serverId;
	std::mutex	m_lock;
};
#endif