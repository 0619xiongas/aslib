#include "../../include/thread/asWorkThread.h"
#include <functional>
asWorkThread::asWorkThread()
	:m_exit(false)
{
}

asWorkThread::~asWorkThread()
{
}

bool asWorkThread::StartThread()
{
	if (m_thread) return false;
	m_thread = new std::thread(std::bind(&asWorkThread::ThreadFunc, this));
	if (m_thread)
	{
		return true;
	}
	return false;
}

bool asWorkThread::IsNeedExit() const
{
	return m_exit;
}

void asWorkThread::Exit()
{
	m_exit = true;
}
