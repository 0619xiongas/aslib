#include "../../include/thread/asMsgThread.h"

asMsgThread::asMsgThread()
	:m_maxCount(100000),m_offset(0)
{
}

asMsgThread::~asMsgThread()
{
	ExitThread();
}

void asMsgThread::PostMsg(asMsg& msg)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_msgs[m_offset].push_back(msg);
	}
	m_cond.notify_all();
}

void asMsgThread::PostMsg(u32 msgID, u32 subID, ulint param1, ulint param2, ulint param3)
{
	asMsg msg(msgID, subID, param1, param2, param3);
	PostMsg(msg);
}

void asMsgThread::ExitThread()
{
	if (!IsNeedExit())
	{
		asMsg msg;
		msg.msgID = AS_THREAD_STOP;
		PostMsg(msg);
		asBaseThread::Sleep(100);
		StopThread();
	}
}

void asMsgThread::SetMsgCount(u32 maxSize)
{
	m_maxCount = maxSize;
}

void asMsgThread::GetMsgCount(u32& list1, u32& list2)
{
	list1 = (u32)m_msgs[0].size();
	list1 = (u32)m_msgs[1].size();
}

void asMsgThread::ThreadFunc()
{
	OnThreadStart();
	while (!IsNeedExit())
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		if (m_msgs[m_offset].size() == 0)
		{
			m_cond.wait(lock);
			continue;
		}
		std::list<asMsg>* tmpList = nullptr;
		tmpList = &(m_msgs[m_offset]);
		m_offset = (m_offset + 1) % 2;
		lock.unlock();
		for (auto itr = tmpList->begin(); itr != tmpList->end(); ++itr)
		{
			switch (itr->msgID)
			{
			case AS_THREAD_STOP:
				Exit();
				break;
			case AS_THREAD_CLEAR:
				{
					std::unique_lock<std::mutex> lc(m_mutex);
					m_msgs[0].clear();
					m_msgs[1].clear();
					lc.unlock();
				}
				break;
			default:
				HandleMessage(*itr);
				break;
			}
		}
		tmpList->clear();
	}
	OnThreadStop();
}

void asMsgThread::OnThreadStart()
{
}

void asMsgThread::OnThreadStop()
{
}

void asMsgThread::HandleMessage(asMsg& msg)
{
	return;
}
