/*************************************************************************************
 *
 * 文 件 名:		asIniReader.h
 * 描    述:		读取 ini 配置文件
 * 
 * 版    本：	V1.0
 * 创 建 者：	astronaut
 * 创建时间：	2025/4/3 16:17
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
	// 打开文件 加载ini配置到m_cfg中
	bool Open(const char* path);
	// 是否存在这个块
	bool HasSection(const char* section);

	// 获取数据
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

