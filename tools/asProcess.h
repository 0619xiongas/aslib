/*************************************************************************************
 *
 * �� �� ��:   asProcess.h
 * ��    ��:		��ǰ���������Ϣ
 * 
 * ��    ����  V1.0
 * �� �� �ߣ�  astronaut
 * ����ʱ�䣺  2025/3/22 12:38
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
	std::string m_fullPath; // ����·��
	std::string m_path;		// �������ڵ�·��
	std::string m_name;		// ������
	long m_pid;				// ����id
};
#endif
