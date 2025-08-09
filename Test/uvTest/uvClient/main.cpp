#include <iostream>
#include "ucTestClient.h"
#include "single/asSingle.h"
#include <memory>
#include <chrono>
#include <thread>
#include "thread/asBaseThread.h"
#include <csignal>
#include <google/protobuf/message.h>
#include "buffer/asBuffer.h"
#include "log/asLogger.h"
#include "vld.h"

auto g_client = asSingleton<uvTestClient>::instance();
bool g_exit = false;
void SignalHandler(int signal) 
{
	if (signal == SIGINT) 
	{
		std::cout << "exiting ..." << std::endl;
		g_exit = true;
	}
}

int main()
{
	signal(SIGINT, SignalHandler);
	AS_LOGGER->Init(nullptr);
	g_client->Init("127.0.0.1", 10011, 1024 * 10, 1024 * 10, 1, 100);
	g_client->TryRunNetWork(true);
	while (!g_exit) {
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
		if (elapsed < 1000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 - elapsed));
		}
	}
	google::protobuf::ShutdownProtobufLibrary();
	asSingleton<uvTestClient>::instance()->TryStopNetWork();
	asSingleton<uvTestClient>::delete_instance();
	asSingleton<asLogger>::instance()->ExitThread();
	asSingleton<asLogger>::delete_instance();
	system("pause");
	return 0;
}