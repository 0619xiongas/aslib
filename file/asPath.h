/*************************************************************************************
 *
 * �� �� ��:   asPath.h
 * ��    ��:	   �����ļ�·����صĲ���
 *
 * ��    ����  V1.0
 * �� �� �ߣ�  astronauts
 * ����ʱ�䣺  2025/3/21 22:18
 * ======================================
 *************************************************************************************/
#ifndef AS_PATH_H
#define AS_PATH_H

#include <string>
class asPath
{
public:
	// ȥ���ļ���
	static std::string RemoveFileName(const char* path);
	// ȥ���ļ���չ��
	static std::string RemoveFileExtend(const char* path);
	// ��ȡ�ļ�����չ��
	static std::string GetFileExtend(const char* path);
	// ��ȡ·�����ļ��� extend �Ƿ�����ļ���չ��
	static std::string GetFileBaseName(const char* path, bool ext);
	// �ļ����Ƿ����
	static bool IsDirExist(const char* path);
	// ����һ���ļ���
	static bool MakeDir(const char* path);
};

#endif // !AS_PATH_h


