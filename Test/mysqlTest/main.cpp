#include <iostream>
#include "log/asLogger.h"
#include <csignal>
#include "DBStmtThreadGroup.h"
#include <chrono>
bool g_exit = false;

void SignalHandler(int signal) {
	if (signal == SIGINT) {
		std::cout << "exiting ..." << std::endl;
		g_exit = true;
	}
}

class PrintThread : public asBaseThread
{
public:
	virtual void ThreadFunc() override
	{
		while (!IsNeedExit())
		{
			auto start = std::chrono::steady_clock::now();

			// 打印信息
			asSingleton<DBStmtThreadGroup>::instance()->PrintDBInfo();

			// 计算剩余等待时间（确保精确3秒间隔）
			auto end = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			if (elapsed < 1000) {
				std::this_thread::sleep_for(std::chrono::milliseconds(1000 - elapsed));
			}
		}
	}
};
int main()
{
	PrintThread* thread = new PrintThread();
	if (!thread)
	{
		return -1;
	}
	signal(SIGINT, SignalHandler);
	AS_LOGGER->Init(nullptr);

	asSingleton<DBStmtThreadGroup>::instance()->StartGroup("localhost", 3306, "root", "123456", "test_db", "utf8", false);
	thread->StartThread();

	while (!g_exit)
	{

	}

	asSingleton<asLogger>::delete_instance();
	asSingleton<DBStmtThreadGroup>::delete_instance();
	thread->StopThread();
	system("pause");
	return 0;
}