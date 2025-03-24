/*************************************************************************************
 *
 * �� �� ��:   asCFileWriter.h
 * ��    ��:		�ļ�д�룬����c����FILE����д��
 * 
 * ��    ����  V1.0
 * �� �� �ߣ�  astronauts
 * ����ʱ�䣺  2025/3/24 22:33
 * ======================================
*************************************************************************************/
#ifndef AS_C_FILEWRITER_H
#define AS_C_FILEWRITER_H

#include "../asBaseDefine.h"
#include <string>
#include <cstdio>

// ʹ��c��׼FILE����д��

class asCFileWriter
{
public:
	asCFileWriter();
	~asCFileWriter();
	bool Init(const char* path, bool app, bool brief, bool binary);
	// end ���"\n" time �Ƿ���Ҫ���ʱ����Ϣ
	bool WriteString(const char* str, bool end, bool time);
	bool WriteBytes(const char* bytes, u32 len);
	bool OpenFile();
	void CloseFile();
private:
	bool 			m_app;		// �ļ�׷��д 
	bool 			m_brief;	// �Ƿ�д�������ر�
	bool 			m_binary;	// �Ƿ��Ƕ������ļ�
	std::string 	m_path;		// �ļ�·��
	FILE* m_pFile;	// �ļ�ָ��
};

#endif