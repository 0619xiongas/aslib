#include <iostream>
#include "uvTestServer.h"
#include "single/asSingle.h"
#include <thread>
#include "thread/asBaseThread.h"
#include <vld.h>
#include <csignal>
//#include <google/protobuf/stubs/common.h>
auto g_server = asSingleton<uvTestServer>::instance();
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
	g_server->Init("0,0,0,0", 10011, 1024 * 10, 1024 * 10, 4,1000);
	g_server->TryRunNetWork(false);


	while (!g_exit)
	{

		auto start = std::chrono::steady_clock::now();
		auto end = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (elapsed < 1000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 - elapsed));
		}
	}
	//google::protobuf::ShutdownProtobufLibrary();
	asSingleton<uvTestServer>::instance()->TryStopNetWork();
	asSingleton<uvTestServer>::delete_instance();
	system("pause");
	return 0;
}