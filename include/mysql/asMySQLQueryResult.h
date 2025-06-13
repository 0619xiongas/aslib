#ifndef AS_MYSQLDEFINE_H
#define AS_MYSQLDEFINE_H

#ifdef _WIN32
#include <mysql.h>
#else
#include <mysql/mysql.h>
#endif
#include "asMySQLDefine.h"
#include <string>
#include <functional>
#include <vector>

// 查询结果集
class asMySQLQueryResult
{
public:
	asMySQLQueryResult();
	~asMySQLQueryResult();

	// 设置当前MYSQL_RES结果，对于结果集数组为空或者超出则设置nullptr
	void NextResult();

	void AddResults(MYSQL_RES* res);

	// 是否还有数据
	bool IsEof();
	// 下一条数据
	bool NextRow();
	// 定位到offset 行数据
	bool SeekRow(u32 offset);
	// 获取返回结果行数
	u32 GetRowCount();
	// 获取返回结果列数
	u32 GetColumnCount();

	const char* GetStringValue(u32 column, const char* defaultStr = "");
	const char* GetStringValue(const char* name, const char* defaultStr = "");

	i32 GetIntValue(u32 column, const int defaultV = 0);
	i32 GetIntValue(const char* name, const int defaultV = 0);

	i64 GetI64Value(u32 column, const i64 defaultV = 0l);
	i64 GetI64Value(const char* name, const i64 defaultV = 0l);

	f64 GetDoubleValue(u32 column, const f64 defaultV = 0.0f);
	f64 GetDoubleValue(const char* name, const f64 defaultV = 0.0f);

	bool GetBlobValue(u32 column, void*& out, unsigned long& len);
	bool GetBlobValue(const char* name, void*& out, unsigned long& len);

	int GetColumnIndex(const char* name);
	const char* GetColumnName(u32 column, const char* defaultStr);

	enum_field_types GetColumnType(const char* name);
	enum_field_types GetColumnType(u32 column);

	std::string& GetError();
	bool IsSuccess();
	bool IsEndResults();

private:
	void FreeResults();
	void FreeCurResult();
public:
	std::vector<MYSQL_RES*> m_results;
	MYSQL_RES* m_curRes;
	MYSQL_ROW m_curRow;
	u32 m_columns;
	u32 m_rows;
	unsigned long* m_curLen; // 当前行，每个字段数据所占的数据实际长度（字节）
	u32 m_resIndex;
	bool m_success;
	std::string m_error;
};

typedef std::function<void(asMySQLQueryResult& res)> MySQLResultCB;

struct asMySQLCmdParam
{
	MySQLResultCB cb;
	std::string sql;
	asMySQLCmdType type;
	std::shared_ptr<asMySQLQueryResult> res;
};
#endif
