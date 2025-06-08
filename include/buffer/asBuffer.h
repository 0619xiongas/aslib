/*************************************************************************************
 *
 * 文 件 名:		asBuffer.h
 * 描    述:		
 * 
 * 版    本：	V1.0
 * 创 建 者：	astronaut
 * 创建时间：	2025/5/28 10:44
 * ======================================
*************************************************************************************/

#ifndef AS_BUFFER_H
#define AS_BUFFER_H
#include "../asBaseDefine.h"
#include <cstring>


enum class BUFFER_DATA_TYPE
{
	BDT_NULL = 0,
	BDT_EXTERNAL = 1, // 外部传入的空间
	BDT_NEED_NEW = 2, // 内部开辟空间
};
class asBuffer
{
public:
	asBuffer();
	asBuffer(const asBuffer& other);
	asBuffer(ulint size);
	void operator = (const asBuffer& other);
	~asBuffer();

	// 内部初始化一块内存
	bool Init(ulint size, BUFFER_DATA_TYPE type = BUFFER_DATA_TYPE::BDT_NEED_NEW);
	// 外部的内存
	bool Init(char* data, ulint size);
	// 当前size
	ulint Size() const;
	// 最大size
	ulint MaxSize() const;
	// m_data地址
	char* Buf() const;
	// 当前可用缓存的地址
	char* CurrentBuf();
	// 缓存写入data
	bool WriteBuf(const void* data, ulint len);
	// 重新分配大小 一般用于扩容
	bool Resize(ulint newLen, bool saveData);
	// 缓存是否满了
	bool IsFull() const;
	//Buf 数据地址来源
	i32 BufType() const;
	// buf前移
	bool MoveDataForWard(ulint offset);
	// 数据清空
	void Reset(bool Zero = false);
	// 可用空间
	ulint RemainSize() const;
	// 裁剪数据
	bool SubData(ulint offset, ulint len);

	bool AddSize(ulint size);

	bool SubSize(ulint size);

	bool SetSize(ulint size);

	void Clear();
private:
	// 拷贝
	void Copy(const asBuffer& other);
	char* m_data;
	u64 m_maxSize;
	u64 m_curSize;
	BUFFER_DATA_TYPE m_type = BUFFER_DATA_TYPE::BDT_NULL;
};
#endif
