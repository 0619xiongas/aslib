#pragma once
#include <vector>
#include "asBaseDefine.h"
class SQLProducer
{
public:
	SQLProducer();
	~SQLProducer();

public:

	void DoSQLUpdate();

	void DoSQLSelect();

	void DoSQLDelete();

	void OnSQL_CB();
private:
	void OnSQLUpdate_CB();

	void OnSQLSelect_CB();

	void OnSQLDelete_CB();
	std::vector<i64> m_ids;
	u32 m_idx = 0;
};