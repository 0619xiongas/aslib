/*************************************************************************************
 *
 * 文 件 名:   asTime.h
 * 描    述:		用于获取时间和时间戳 时间由于存在UTC时间和本地时间，通常本地时间 = UTC + 时区
 *				比如北京时间 = UTC时间 + 8小时，使用gmtime(time_t timestamp)获取的tm结构体是根据timestamp
 *				决定是utc时间还是本地时间。
 * 
 * 版    本：  V1.0
 * 创 建 者：  astronauts
 * 创建时间：  2025/3/21 22:52
 * ======================================
*************************************************************************************/
#ifndef AS_TIME_H
#define AS_TIME_H

#ifndef _WIN32
#include <sys/time.h>
#endif
#include <time.h>
#include "../asBaseDefine.h"

// 格式化时间的最大长度
#define TIME_STR_MAX_LEN 32
struct YMDHMS
{
	Union32 year;
	Union32 month;
	Union32 day;
	Union32 wday; // 周几
	Union32 yday; // 一年的第几天
	Union32 hour;
	Union32 minutes;
	Union32 second;

	void Zero();
	bool IsZero();
	void GetLocalTime(char* buf);
	Union64 GetTimeStamp();
};

class asTime
{
public:
	// 从1970.1.1开始，获取当前的时间戳秒 / 毫秒Ms
	static Union64 GetTimeNow();
	static Union64 GetTimeNowMs();
	// 获取当前时间 UTC时间
	static tm* GetTime();
	static tm* GetTime(time_t timestamp);
	static void GetTime(YMDHMS& t);
	static void GetTime(time_t timestamp, YMDHMS& t);
	static time_t GetTimeStamp(YMDHMS& t);
	// 获取本地时间
	static tm* GetLocalTime();
	static tm* GetLocalTime(time_t timestamp);
	static void GetLocalTime(YMDHMS& t);
	static void GetLocalTime(time_t timestamp, YMDHMS& t);


private:
	static time_t s_timeZone;// 时区 用东八区
	static time_t s_sysZone; // 系统所在的时区
	static time_t s_todayZero; // 今日零点时间。每天刷新
	static time_t s_sysStart; // 进程启动时间戳
	static time_t s_timeOffset; // 时间偏移量
};

#endif 

