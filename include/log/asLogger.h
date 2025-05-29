/*************************************************************************************
 *
 * �� �� ��:		asLogger.h
 * ��    ��:		ʵ��һ���̵߳���־�����ӡ��
 * 
 * ��    ����	V1.0
 * �� �� �ߣ�	astronaut
 * ����ʱ�䣺	2025/5/29 9:27
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
	// ֻ��ӡ������̨
	void PrintConsole(LOGTYPE type, const char* str);
	void PrintConsoleEx(LOGTYPE type, const char* str, ...);
	// ֻд�뵽�ļ�
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
	char			m_type; // �ļ���Ƭ����
	char			m_lastTime[32]; // ��һ�δ���log�ļ���ʱ��

};

#define	AS_LOGGER asSingleton<asLogger>::instance()

#endif
