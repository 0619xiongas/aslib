/*************************************************************************************
 *
 * 文 件 名:		asLogger.h
 * 描    述:		实现一个线程的日志输出打印类
 * 
 * 版    本：	V1.0
 * 创 建 者：	astronaut
 * 创建时间：	2025/5/29 9:27
 * ======================================
*************************************************************************************/
#ifndef AS_LOGGER_H
#define AS_LOGGER_H
#include "../thread/asMsgThread.h"
#include "../tools/asProcess.h"
#include "../file/asCFileWriter.h"
#include <stdarg.h>

enum LOGTYPE
{
	TIP,
	BUG,
	ERR,
};

class asLogger : public asMsgThread
{
public:
	asLogger();
	~asLogger();
	bool Init(char type, const char* path);

	void Log(LOGTYPE type, const char* str);
	void LogEx(LOGTYPE type, const char* str, ...);
	// 只打印到控制台
	void PrintConsole(LOGTYPE type, const char* str);
	void PrintConsoleEx(LOGTYPE type, const char* str, ...);
	// 只写入到文件
	void PrintFile(LOGTYPE type, const char* str);
	void PrintFileEx(LOGTYPE type, const char* str, ...);
private:
	void FormatString(LOGTYPE type, const char* str,u32 lv);
	void FormatStringEx(LOGTYPE type, const char* str, u32 lv,va_list& vl);
	void SaveToFile(const char* log);
	void CheckMakeFile();
	virtual void HandleMessage(asMsg& msg) override;
	std::string GetTypeString(LOGTYPE type);
private:
	asProcess		m_info;
	asCFileWriter	m_cfw;
	std::string		m_dir;
	char			m_type; // 文件分片类型
	char			m_lastTime[32]; // 上一次创建log文件的时间

};

#define	AS_LOGGER asSingleton<asLogger>::instance()

#endif
