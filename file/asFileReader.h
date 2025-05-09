#ifndef AS_FILEREADER_H
#define AS_FILEREADER_H
#include "../buffer/asBuffer.h"
#include <fstream>

// 读取文件存入buffer中 可作为基类
class asFileReader
{
public:
    asFileReader();
    ~asFileReader();

    void SetPath(const char* path);

    void Clear();

    bool OpenFile(bool b = false);

    void CloseFile();
    // 获取文件大小
    ulint GetFileSize();
    // 从文件获取一行数据，传入到buffer中
    bool GetLine();
    // 读取文件数据到内部缓冲区
    bool ReadAll();

    asBuffer& GetInnerBuffer();
private:
    bool CreateBuffer();
private:
    asBuffer        m_buffer; // 内部缓冲区
    std::string     m_path;
    std::ifstream   m_ifs;
};

#endif