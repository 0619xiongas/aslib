#include <iostream>
#include "ucTestClient.h"
#include "single/asSingle.h"
#include <memory>
#include <chrono>
#include <thread>

std::shared_ptr<uvTestClient> g_client = asSingle<uvTestClient>::instance();

int main()
{
	g_client->Init("127.0.0.1", 10011, 1024 * 10, 1024 * 10, 1, 100);
	g_client->TryRunNetWork(true);
	while (true) {
		auto start = std::chrono::steady_clock::now();

		// 打印信息
		u32 sid = g_client->GetServerSession();
		if (sid != 0)
		{
			g_client->DoTestSendData();
		}

		// 计算剩余等待时间（确保精确3秒间隔）
		auto end = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (elapsed < 100) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100 - elapsed));
		}
	}
	return 0;
}