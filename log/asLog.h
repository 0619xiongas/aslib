/*************************************************************************************
 *
 * �� �� ��:   asLog.h
 * ��    ��:	  ��־��� ���߳��� Ŀǰʹ�õ��Ǳ�׼cд�� ����дһ���ļ���һ��
 * 
 * ��    ����  V1.0
 * �� �� �ߣ�  astronaut
 * ����ʱ�䣺  2025/3/29 9:04
 * ======================================
*************************************************************************************/
#ifndef AS_LOG_H
#define AS_LOG_H
#include "../file/asCFileWriter.h"
#include "../tools/asProcess.h"
#include <stdarg.h>
enum LOGTYPE
{
	INFO,
	DEBUG,
	WARNING,
	ERROR,
	MAX,
};
#define LOG_STR_MAX 1024
class asLog
{
public:
	explicit asLog(char sub, const char* path);
	~asLog();

	void SetSubSection(char type);
	void Log(LOGTYPE type, const char* str);
	void LogEx(LOGTYPE type, const char* str, ...);

private:
	void FormatString(LOGTYPE type, const char* str);
	void FormatStringEx(LOGTYPE type, const char* str, va_list& valist);
	void SaveToFile();
	void CheckMakeFile();
private:
	asProcess		m_info;		// ����
	asCFileWriter	m_cfw;		// �ļ�д��
	std::string		m_dir;
	char 			m_subsection; // ��־�ļ���Ƭ���ͣ����죬��Сʱ
	char			m_lastTime[32]; // ��һ�δ���log�ļ���ʱ��
	std::string		m_buffer; 	//��־����
};

#endif