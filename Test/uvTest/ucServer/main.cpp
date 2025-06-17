#include <iostream>
#include "uvTestServer.h"
#include "single/asSingle.h"
#include <thread>
#include "thread/asBaseThread.h"
#include <vld.h>
#include <csignal>
#include <google/protobuf/stubs/common.h>
auto g_server = asSingleton<uvTestServer>::instance();

void SignalHandler(int signal) {
	if (signal == SIGINT) {
		std::cout << "捕获到 Ctrl+C，退出中..." << std::endl;
		exit(0);
	}
}

int main()
{
	signal(SIGINT, SignalHandler);  // 注册信号处理器
	g_server->Init("0,0,0,0", 10011, 1024 * 10, 1024 * 10, 4,1000);
	g_server->TryRunNetWork(false);


	while (1)
	{

		auto start = std::chrono::steady_clock::now();

		// 打印信息

		// 计算剩余等待时间（确保精确3秒间隔）
		auto end = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (elapsed < 1000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 - elapsed));
		}
	}
	google::protobuf::ShutdownProtobufLibrary();
	asSingleton<uvTestServer>::delete_instance();
	system("pause");
	return 0;
}