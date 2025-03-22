#pragma once
#include "../asBaseDefine.h"
#include <cstring>


enum class BUFFER_DATA_TYPE
{
	BDT_NULL = 0,
	BDT_EXTERNAL = 1, // �ⲿ����Ŀռ�
	BDT_NEED_NEW = 2, // �ڲ����ٿռ�
};
class asBuffer
{
public:
	asBuffer();
	asBuffer(const asBuffer& other);
	asBuffer(ulint size);
	void operator = (const asBuffer& other);
	~asBuffer();

	// �ڲ���ʼ��һ���ڴ�
	bool Init(ulint size, BUFFER_DATA_TYPE type = BUFFER_DATA_TYPE::BDT_NEED_NEW);
	// �ⲿ���ڴ�
	bool Init(char* data, ulint size);
	// ��ǰsize
	ulint Size() const;
	// ���size
	ulint MaxSize() const;
	// m_data��ַ
	char* Buf() const;
	// ��ǰ���û���ĵ�ַ
	char* CurrentBuf();
	// ����д��data
	bool WriteBuf(const void* data, ulint len);
	// ���·����С һ����������
	bool Resize(ulint newLen, bool saveData);
	// �����Ƿ�����
	bool IsFull() const;
	//Buf ���ݵ�ַ��Դ
	i32 BufType() const;
	// bufǰ��
	bool MoveDataForWard(ulint offset);
	// �������
	void Reset(bool Zero = false);
	// ���ÿռ�
	ulint RemainSize() const;
	// �ü�����
	bool SubData(ulint offset, ulint len);

	bool AddSize(ulint size);

	bool SubSize(ulint size);

	void Clear();
private:
	// ����
	void Copy(const asBuffer& other);
	char* m_data;
	u64 m_maxSize;
	u64 m_curSize;
	BUFFER_DATA_TYPE m_type = BUFFER_DATA_TYPE::BDT_NULL;
};
