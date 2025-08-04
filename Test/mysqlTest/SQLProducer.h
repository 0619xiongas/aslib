#pragma once
#include <vector>
#include "asBaseDefine.h"
#include "algorithm/asSnowFlake.h"
class SQLProducer
{
public:
	SQLProducer();
	~SQLProducer();

public:

	void DoSQLUpdate();

	void DoSQLSelect();

	void DoSQLDelete();

	void DoSQLInsert();

	void OnSQL_CB(asNetMsgHead* head, char* data, u32 len);
private:
	void OnSQLSelect_CB(asNetMsgHead* head, char* data, u32 len);
	std::vector<i64> m_ids;
	u32 m_idx = 0;
	u32 m_idxRandom = 0;
	asSnowFlake m_sf;
};