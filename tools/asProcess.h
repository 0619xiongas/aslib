/*************************************************************************************
 *
 * 文 件 名:   asProcess.h
 * 描    述:		当前进程相关信息
 * 
 * 版    本：  V1.0
 * 创 建 者：  astronaut
 * 创建时间：  2025/3/22 12:38
 * ======================================
*************************************************************************************/

#ifndef AS_PROCESS_H
#define AS_PROCESS_H
#include <string>
class asProcess
{
public:
	asProcess();
	~asProcess();

	void GetInfo();
	std::string GetNameNoExtend();
public:
	std::string m_fullPath; // 绝对路径
	std::string m_path;		// 程序所在的路径
	std::string m_name;		// 程序名
	long m_pid;				// 进程id
};
#endif
