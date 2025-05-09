#include "asTime.h"
#include <chrono>
#include <cstring>

time_t asTime::s_sysStart = ::time(NULL);
time_t asTime::s_sysZone = asTime::GetSysTimeZone();
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

void asTime::GetLocalTimeStr(char* str)
{
	tm* ti = GetLocalTime();
	snprintf(str, TIME_STR_MAX_LEN, "%d-%d-%d %d:%d:%d",
		ti->tm_year + 1900,
		ti->tm_mon + 1,
		ti->tm_mday,
		ti->tm_hour,
		ti->tm_min,
		ti->tm_sec);
}

void asTime::GetLocalTimeStrYMD(char* str)
{
	tm* ti = GetLocalTime();
	snprintf(str, TIME_STR_MAX_LEN, "%d-%d-%d", ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday);
}

void asTime::GetLocalTimeStrYMDH(char* str)
{
	tm* ti = GetLocalTime();
	snprintf(str, TIME_STR_MAX_LEN, "%d-%d-%d %d", ti->tm_year + 1900, ti->tm_mon + 1, ti->tm_mday,ti->tm_hour);
}

void asTime::GetLocalTimeStrHMS(char* str)
{
	tm* ti = GetLocalTime();
	snprintf(str, TIME_STR_MAX_LEN, "%d:%d:%d", ti->tm_hour, ti->tm_min, ti->tm_sec);
}

Union32 asTime::GetDateNum(bool localtime)
{
	tm* t = localtime ? GetLocalTime() : GetTime();
	Union32 un;
	un.i32p = (t->tm_year + 1900) * 10000 + t->tm_mon * 100 + t->tm_mday;
	return un;
}

Union32 asTime::GetTimeNum(bool localtime)
{
	tm* t = localtime ? GetLocalTime() : GetTime();
	Union32 un;
	un.i32p = (t->tm_hour + 1900) * 10000 + t->tm_min * 100 + t->tm_sec;
	return un;
}

Union64 asTime::GetDayZero(time_t timestamp)
{
	Union64 un;
	un.i64p = timestamp - ((timestamp + asTime::s_timeZone) % 86400);
	return un;
}

Union64 asTime::GetTodayZero()
{
	return GetDayZero(GetTimeNow().i64p);
}

i32 asTime::GetDaySecDiff(time_t day1, time_t day2)
{
	Union64 d1 = GetDayZero(day1);
	Union64 d2 = GetDayZero(day2);
	return i32(d2.i64p - d1.i64p);
}

i32 asTime::GetDaysDiff(time_t now, time_t last, i32 begintm)
{
	return i32(asTime::GetDays(now - begintm + 86400) -
		asTime::GetDays(last - begintm + 86400));
}

u32 asTime::GetDays(time_t timestamp)
{
	return u32((timestamp + asTime::s_timeZone) % 86400);
}

u32 asTime::GetWeeks(time_t timestamp)
{
	//1970年1月1日是星期四
	return(GetDays(timestamp) + 3) / 7;
}

time_t asTime::GetSysTimeZone()
{
#ifdef _WIN32
	long t = 0;
	::_get_timezone(&t);
	return (time_t)t;
#else
	time_t timestamp = time(0);
	tm* timeinfo = localtime(&timestamp);
	return (time_t)timeinfo->tm_gmtoff;
#endif
}

