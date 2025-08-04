#include <iostream>
#include "log/asLogger.h"
#include <csignal>
#include "DBStmtThreadGroup.h"
#include "SQLProducer.h"
#include <chrono>
#include "vld.h"
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

	asSingleton<DBStmtThreadGroup>::instance()->StartGroup("localhost", 3306, "root", "123456", "test_db", "utf8", false);
	std::string cmd;
	while (!g_exit)
	{
		std::getline(std::cin, cmd);
		if (cmd == "s")
		{
			asSingleton<SQLProducer>::instance()->DoSQLSelect();
		}
		else if (cmd == "u")
		{
			asSingleton<SQLProducer>::instance()->DoSQLUpdate();
		}
		else if (cmd == "i")
		{
			asSingleton<SQLProducer>::instance()->DoSQLInsert();
		}
		else if (cmd == "d")
		{
			asSingleton<SQLProducer>::instance()->DoSQLDelete();
		}
		else if (cmd == "p")
		{
			asSingleton<DBStmtThreadGroup>::instance()->PrintDBInfo();
		}
	}

	asSingleton<asLogger>::delete_instance();
	asSingleton<DBStmtThreadGroup>::delete_instance();
	asSingleton<SQLProducer>::delete_instance();
	system("pause");
	return 0;
}