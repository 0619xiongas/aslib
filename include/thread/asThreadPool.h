#ifndef AS_THREADPOOL_H
#define AS_THREADPOOL_H
#include "../asBaseDefine.h"

template<class THREAD>
class asThreadPool
{
public:
	asThreadPool()
	{
		m_threads = nullptr;
		m_count = 0;
		m_poolID = 0
	}
	asThreadPool(u32 count)
	{
		m_threads = nullptr;
		m_count = count;
		m_poolID = 0;
	}
	virtual ~asThreadPool()
	{

	}
protected:
	THREAD*		m_threads;
	u32			m_count;
public:
	u32			m_poolID;
};
#endif