/*************************************************************************************
 *
 * �� �� ��:		asFileReader.h
 * ��    ��:		��ȡ�ļ�����
 * 
 * ��    ����	V1.0
 * �� �� �ߣ�	astronaut
 * ����ʱ�䣺	2025/4/3 10:10
 * ======================================
*************************************************************************************/
#ifndef AS_FILEREADER_H
#define AS_FILEREADER_H
#include "../buffer/asBuffer.h"
#include <fstream>

// ��ȡ�ļ�����buffer�� ����Ϊ����
class asFileReader
{
public:
	asFileReader();
	~asFileReader();

	void SetPath(const char* path);

	void Clear();

	bool OpenFile(bool b = false);

	void CloseFile();
	// ��ȡ�ļ���С
	ulint GetFileSize();
	// ���ļ���ȡһ�����ݣ����뵽buffer��
	bool GetLine();
	// ��ȡ�ļ����ݵ��ڲ�������
	bool ReadAll();

	asBuffer& GetInnerBuffer();
private:
	bool CreateBuffer();
private:
	asBuffer        m_buffer; // �ڲ�������
	std::string     m_path;
	std::ifstream   m_ifs;
};

#endif
