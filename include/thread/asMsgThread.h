#ifndef AS_MSGTHREAD_H
#define AS_MSGTHREAD_H

#include "asWorkThread.h"
#include <list>
#include <mutex>
#include <condition_variable>
#include "../asMsgBaseDefine.h"
using namespace asThread;
class asMsgThread : public asWorkThread
{
public:
	asMsgThread();
	virtual ~asMsgThread();

	// 向此线程投递消息
	void PostMsg(asMsg& msg);
	void PostMsg(u32 msgID, u32 subID, ulint param1, ulint param2, ulint param3); 
	void ExitThread();
	//设置消息队列最大值
	void SetMsgCount(u32 maxSize);
	//获取当前线程消息数
	void GetMsgCount(u32& list1,u32 &list2);
protected:
	virtual void ThreadFunc();

	virtual void OnThreadStart();

	virtual void OnThreadStop();

	virtual void HandleMessage(asMsg& msg);
protected:
	std::list<asMsg>	m_msgs[2];
	std::mutex			m_mutex;
	std::condition_variable	m_cond;
	u8					m_offset;
	u32					m_maxCount;
};

#endif
