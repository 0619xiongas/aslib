#include "asTime.h"
#include <chrono>
#include <cstring>

time_t asTime::s_sysStart = ::time(NULL);
time_t asTime::s_sysZone = 8 * 60 * 60;
time_t asTime::s_timeZone = 8 * 60 * 60;
time_t asTime::s_timeOffset = 0;
time_t asTime::s_todayZero = 0;

void YMDHMS::Zero()
{
	year.i32p = 0;
	month.i32p = 0;
	day.i32p = 0;
	yday.i32p = 0;
	wday.i32p = 0;
	hour.i32p = 0;
	minutes.i32p = 0;
	second.i32p = 0;
}

bool YMDHMS::IsZero()
{
	return false;
}

void YMDHMS::GetLocalTime(char* buf)
{
}

Union64 YMDHMS::GetTimeStamp()
{
	return Union64();
}


Union64 asTime::GetTimeNow()
{
	Union64 ret;
	ret.i64p = ::time(NULL);
	return ret;
}

Union64 asTime::GetTimeNowMs()
{
	Union64 ret;
	ret.i64p = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count();
	return ret;
}

tm* asTime::GetTime()
{
	time_t t = GetTimeNow().i64p;
	struct tm* ret = ::gmtime(&t);
	return ret;
}

tm* asTime::GetTime(time_t timestamp)
{
	struct tm* ret = ::gmtime(&timestamp);
	return ret;
}

void asTime::GetTime(YMDHMS& t)
{
	tm* ti = GetTime();
	t.year.i32p = ti->tm_year + 1900;
	t.month.i32p = ti->tm_mon + 1;
	t.day.i32p = ti->tm_mday;
	t.yday.i32p = ti->tm_yday;
	t.wday.i32p = ti->tm_wday;
	t.hour.i32p = ti->tm_hour;
	t.minutes.i32p = ti->tm_min;
	t.second.i32p = ti->tm_sec;
}

void asTime::GetTime(time_t timestamp, YMDHMS& t)
{
	tm* ti = GetTime(timestamp);
	t.year.i32p = ti->tm_year + 1900;
	t.month.i32p = ti->tm_mon + 1;
	t.day.i32p = ti->tm_mday;
	t.yday.i32p = ti->tm_yday;
	t.wday.i32p = ti->tm_wday;
	t.hour.i32p = ti->tm_hour;
	t.minutes.i32p = ti->tm_min;
	t.second.i32p = ti->tm_sec;
}

time_t asTime::GetTimeStamp(YMDHMS& t)
{
	tm ti = { 0 };
	ti.tm_year = t.year.i32p - 1900;
	ti.tm_mon = t.month.i32p - 1;
	ti.tm_mday = t.day.i32p;
	ti.tm_hour = t.hour.i32p;
	ti.tm_min = t.minutes.i32p;
	ti.tm_sec = t.second.i32p;
	return ::mktime(&ti);
}

tm* asTime::GetLocalTime()
{
	time_t t = GetTimeNow().i64p + asTime::s_timeZone;
	return ::gmtime(&t);
}

tm* asTime::GetLocalTime(time_t timestamp)
{
	timestamp += asTime::s_timeZone;
	return ::gmtime(&timestamp);
}

void asTime::GetLocalTime(YMDHMS& t)
{
	tm* ti = GetLocalTime();
	t.year.i32p = ti->tm_year + 1900;
	t.month.i32p = ti->tm_mon + 1;
	t.day.i32p = ti->tm_mday;
	t.wday.i32p = ti->tm_wday;
	t.yday.i32p = ti->tm_yday;
	t.hour.i32p = ti->tm_hour;
	t.minutes.i32p = ti->tm_min;
	t.second.i32p = ti->tm_sec;
}

void asTime::GetLocalTime(time_t timestamp, YMDHMS& t)
{
	timestamp += asTime::s_timeZone;
	tm* ti = gmtime(&timestamp);
	t.year.i32p = ti->tm_year + 1900;
	t.month.i32p = ti->tm_mon + 1;
	t.day.i32p = ti->tm_mday;
	t.wday.i32p = ti->tm_wday;
	t.yday.i32p = ti->tm_yday;
	t.hour.i32p = ti->tm_hour;
	t.minutes.i32p = ti->tm_min;
	t.second.i32p = ti->tm_sec;
}

