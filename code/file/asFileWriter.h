/*************************************************************************************
 *
 * 文 件 名:   asFileWriter.h
 * 描    述:		写入文件 采用c++标准文件流
 * 
 * 版    本：  V1.0
 * 创 建 者：  astronauts
 * 创建时间：  2025/3/24 22:30
 * ======================================
*************************************************************************************/
#ifndef AS_FILEWRITER_H
#define AS_FILEWRITER_H

#include<fstream>
#include "../asBaseDefine.h"
#include <string>

class asFileWriter
{
public:
	asFileWriter();
	~asFileWriter();
	// 初始化或者重置
	bool Init(const char* path, bool app, bool brief, bool binary);
	// end 添加"\n" time 是否需要添加时间信息
	bool WriteString(const char* str, bool end, bool time);
	bool WriteBytes(const char* bytes, u32 len);
	bool OpenFile();
	void CloseFile();

private:
	bool 			m_app;		// 文件追加写 std::ios::app,默认std::ios::ate
	bool 			m_brief;	// 是否写完立即关闭
	bool 			m_binary;	// 是否是二进制文件
	std::string 	m_path;		// 文件路径
	std::ofstream 	m_fs; 		// 文件写入流
};

#endif