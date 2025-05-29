#ifndef AS_WORKTHREAD_H
#define AS_WORKTHREAD_H

#include "asBaseThread.h"

class asWorkThread : public asBaseThread
{
public:
	asWorkThread();
	virtual ~asWorkThread();

	bool StartThread();

	void Exit();
protected:
	virtual void ThreadFunc() = 0;
	bool IsNeedExit() const;
protected:
	bool m_exit;
};
#endif
