#include <iostream>
#include "asioTestServer.h"
#include "single/asSingle.h"
#include <thread>
#include "thread/asBaseThread.h"
#include <csignal>
#include <google/protobuf/message.h>
#include "log/asLogger.h"
#include "vld.h"
auto g_server = asSingleton<asioTestServer>::instance();
bool g_exit = false;
void SignalHandler(int signal) {
	if (signal == SIGINT) {
		std::cout << "exiting ..." << std::endl;
		g_exit = true;
	}
}

int main()
{
	signal(SIGINT, SignalHandler);
	AS_LOGGER->Init(nullptr);
	g_server->Init("0.0.0.0", 10011, 1024 * 10, 1024 * 10, 4,1000);
	g_server->TryRunNetwork(false);


	while (!g_exit)
	{

		auto start = std::chrono::steady_clock::now();
		auto end = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (elapsed < 1000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 - elapsed));
		}
	}
	google::protobuf::ShutdownProtobufLibrary();
	asSingleton<asioTestServer>::instance()->TryStopNetwork();
	asSingleton<asioTestServer>::delete_instance();
	asSingleton<asLogger>::instance()->ExitThread();
	asSingleton<asLogger>::delete_instance();
	system("pause");
	return 0;
}