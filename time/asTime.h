/*************************************************************************************
 *
 * �� �� ��:   asTime.h
 * ��    ��:		���ڻ�ȡʱ���ʱ��� ʱ�����ڴ���UTCʱ��ͱ���ʱ�䣬ͨ������ʱ�� = UTC + ʱ��
 *				���籱��ʱ�� = UTCʱ�� + 8Сʱ��ʹ��gmtime(time_t timestamp)��ȡ��tm�ṹ���Ǹ���timestamp
 *				������utcʱ�仹�Ǳ���ʱ�䡣
 * 
 * ��    ����  V1.0
 * �� �� �ߣ�  astronauts
 * ����ʱ�䣺  2025/3/21 22:52
 * ======================================
*************************************************************************************/
#ifndef AS_TIME_H
#define AS_TIME_H

#ifndef _WIN32
#include <sys/time.h>
#endif
#include <time.h>
#include "../asBaseDefine.h"

// ��ʽ��ʱ�����󳤶�
#define TIME_STR_MAX_LEN 32
struct YMDHMS
{
	Union32 year;
	Union32 month;
	Union32 day;
	Union32 wday; // �ܼ�
	Union32 yday; // һ��ĵڼ���
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
	// ��1970.1.1��ʼ����ȡ��ǰ��ʱ����� / ����Ms
	static Union64 GetTimeNow();
	static Union64 GetTimeNowMs();
	// ��ȡ��ǰʱ�� UTCʱ��
	static tm* GetTime();
	static tm* GetTime(time_t timestamp);
	static void GetTime(YMDHMS& t);
	static void GetTime(time_t timestamp, YMDHMS& t);
	static time_t GetTimeStamp(YMDHMS& t);
	// ��ȡ����ʱ��
	static tm* GetLocalTime();
	static tm* GetLocalTime(time_t timestamp);
	static void GetLocalTime(YMDHMS& t);
	static void GetLocalTime(time_t timestamp, YMDHMS& t);


private:
	static time_t s_timeZone;// ʱ�� �ö�����
	static time_t s_sysZone; // ϵͳ���ڵ�ʱ��
	static time_t s_todayZero; // �������ʱ�䡣ÿ��ˢ��
	static time_t s_sysStart; // ��������ʱ���
	static time_t s_timeOffset; // ʱ��ƫ����
};

#endif 

