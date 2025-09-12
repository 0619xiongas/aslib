#ifndef AS_LOG_H
#define AS_LOG_H
#include "../file/asCFileWriter.h"
#include "../tools/asProcess.h"
#include <stdarg.h>
enum LOGTYPE
{
	TIP,
	BUG,
	ERR,
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
