/*************************************************************************************
 *
 * 文 件 名:   asLog.h
 * 描    述:	  日志输出 单线程类 目前使用的是标准c写入 且是写一次文件打开一次
 * 
 * 版    本：  V1.0
 * 创 建 者：  astronaut
 * 创建时间：  2025/3/29 9:04
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
	asProcess		m_info;		// 程序
	asCFileWriter	m_cfw;		// 文件写入
	std::string		m_dir;
	char 			m_subsection; // 日志文件分片类型，按天，按小时
	char			m_lastTime[32]; // 上一次创建log文件的时间
	std::string		m_buffer; 	//日志缓存
};

#endif