/*************************************************************************************
 *
 * �� �� ��:   asFileWriter.h
 * ��    ��:		д���ļ� ����c++��׼�ļ���
 * 
 * ��    ����  V1.0
 * �� �� �ߣ�  astronaut
 * ����ʱ�䣺  2025/3/24 22:30
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
	// ��ʼ����������
	bool Init(const char* path, bool app, bool brief, bool binary);
	// end ���"\n" time �Ƿ���Ҫ���ʱ����Ϣ
	bool WriteString(const char* str, bool end, bool time);
	bool WriteBytes(const char* bytes, u32 len);
	bool OpenFile();
	void CloseFile();

private:
	bool 			m_app;		// �ļ�׷��д std::ios::app,Ĭ��std::ios::ate
	bool 			m_brief;	// �Ƿ�д�������ر�
	bool 			m_binary;	// �Ƿ��Ƕ������ļ�
	std::string 	m_path;		// �ļ�·��
	std::ofstream 	m_fs; 		// �ļ�д����
};

#endif