/*************************************************************************************
 *
 * 文 件 名:   asCFileWriter.h
 * 描    述:		文件写入，采用c风格的FILE进行写入
 * 
 * 版    本：  V1.0
 * 创 建 者：  astronauts
 * 创建时间：  2025/3/24 22:33
 * ======================================
*************************************************************************************/
#ifndef AS_C_FILEWRITER_H
#define AS_C_FILEWRITER_H

#include "../asBaseDefine.h"
#include <string>
#include <cstdio>

// 使用c标准FILE进行写入

class asCFileWriter
{
public:
	asCFileWriter();
	~asCFileWriter();
	bool Init(const char* path, bool app, bool brief, bool binary);
	// end 添加"\n" time 是否需要添加时间信息
	bool WriteString(const char* str, bool end, bool time);
	bool WriteBytes(const char* bytes, u32 len);
	bool OpenFile();
	void CloseFile();
private:
	bool 			m_app;		// 文件追加写 
	bool 			m_brief;	// 是否写完立即关闭
	bool 			m_binary;	// 是否是二进制文件
	std::string 	m_path;		// 文件路径
	FILE* m_pFile;	// 文件指针
};

#endif