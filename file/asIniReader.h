/*************************************************************************************
 *
 * �� �� ��:		asIniReader.h
 * ��    ��:		��ȡ ini �����ļ�
 * 
 * ��    ����	V1.0
 * �� �� �ߣ�	astronaut
 * ����ʱ�䣺	2025/4/3 16:17
 * ======================================
*************************************************************************************/
#ifndef AS_INIREADER_H
#define AS_INIREADER_H

#include "asFileReader.h"
#include <map>

typedef std::map<std::string, std::string> K_V;

class asIniReader
{
public:
	asIniReader();
	~asIniReader();
	// ���ļ� ����ini���õ�m_cfg��
	bool Open(const char* path);
	// �Ƿ���������
	bool HasSection(const char* section);

	// ��ȡ����
	std::string& GetString(const char* section, const char* key, const char* defaultV);
	i32 GetInt32(const char* section, const char* key, i32 defaultV);
	i64 GetLong(const char* section, const char* key, i64 defaultV);
	std::map<std::string, K_V>& GetCfg() { return m_cfg; }

private:
	asFileReader				m_file;
	std::map<std::string, K_V>	m_cfg;
	std::string					m_tmp;
};

#endif // !AS_INIREADER_H

