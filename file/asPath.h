/*************************************************************************************
 *
 * 文 件 名:   asPath.h
 * 描    述:	   用于文件路径相关的操作
 *
 * 版    本：  V1.0
 * 创 建 者：  astronauts
 * 创建时间：  2025/3/21 22:18
 * ======================================
 *************************************************************************************/
#ifndef AS_PATH_H
#define AS_PATH_H

#include <string>
class asPath
{
public:
	// 去掉文件名
	static std::string RemoveFileName(const char* path);
	// 去掉文件拓展名
	static std::string RemoveFileExtend(const char* path);
	// 获取文件的拓展名
	static std::string GetFileExtend(const char* path);
	// 获取路径的文件名 extend 是否包含文件拓展名
	static std::string GetFileBaseName(const char* path, bool ext);
	// 文件夹是否存在
	static bool IsDirExist(const char* path);
	// 创建一个文件夹
	static bool MakeDir(const char* path);
};

#endif // !AS_PATH_h


