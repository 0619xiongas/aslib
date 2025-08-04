#ifndef AS_MSGTHREADGROUP_H
#define AS_MSGTHREADGROUP_H
#include "../asMsgBaseDefine.h"
#include <string>

using namespace asThread;

template<class THREAD>
class asMsgThreadGroup
{
public:
	asMsgThreadGroup()
	{
		m_threads = nullptr;
		m_count = 0;
		m_groupID = 0;
	}
	asMsgThreadGroup(u32 count)
	{
		m_threads = nullptr;
		m_count = count;
		m_groupID = 0;
	}
	virtual ~asMsgThreadGroup()
	{
		Clear();
	}
	// �ͷ��ڴ�
	void Clear()
	{
		if (m_threads)
		{
			Exit();
			AS_SAFE_DELETE_ARRAY(m_threads);
			m_threads = nullptr;
		}
	}
	void Exit()
	{
		for (u32 i = 0; i < m_count; ++i)
		{
			m_threads[i].ExitThread();
		}
	}
	void Start()
	{
		for (u32 i = 0; i < m_count; ++i)
		{
			m_threads[i].m_threadId = i;
			m_threads[i].StartThread();
		}
	}
	bool PostMsg(u32 idx, asMsg& msg)
	{
		if (idx < m_count)
		{
			m_threads[idx].PostMsg(msg);
			return true;
		}
		return false;
	}
	bool PostMsg(u32 idx, u32 msgID, u32 subID, ulint param1, ulint param2, ulint param3)
	{
		if (idx < m_count)
		{
			m_threads[idx].PostMsg(msgID, subID, param1, param2, param3);
			return true;
		}
		return false;
	}
	bool PostMsgToMin(asMsg& msg)
	{
		u32 count1 = 0;
		u32 count2 = 0;
		m_threads[0].GetMsgCount(count1, count2);
		u32 total = count1 + count2;
		u32 idx = 0;
		for (u32 i = 1; i < m_count; ++i)
		{
			m_threads[i].GetMsgCount(count1, count2);
			if (total > count1 + count2)
			{
				total = count1 + count2;
				idx = i;
			}
		}
		m_threads[idx].PostMsg(msg);
		return true;
	}
	bool PostMsgToMin(u32 msgID, u32 subID, ulint param1, ulint param2, ulint param3)
	{
		asMsg msg(msgID, subID, param1, param2, param3);
		return PostMsgToMin(msg);
	}
	bool SetThreadGroupNum(u32 num)
	{
		if (!m_threads && num > 0 && num < AS_THREAD_MAX_GROUP_COUNT)
		{
			m_count = num;
			m_threads = new THREAD[m_count];
			return true;
		}
		return false;
	}
	u32 GetGroupNum()
	{
		return m_count;
	}
	void SetThreadsName(const char* name, const wchar_t* wName)
	{
		for (u32 i = 0; i < m_count; ++i)
		{
			std::string n(name);
			n += std::to_string(i);
			std::wstring wn(wName);
			wn += std::to_wstring(i);
			m_threads[i].SetThreadName(n.c_str(), wn.c_str());
		}
	}
protected:
	THREAD*		m_threads;
	u32			m_count;
public:
	u32			m_groupID;
};
#endif