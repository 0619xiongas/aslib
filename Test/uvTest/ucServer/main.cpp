#include <iostream>
#include "uvTestServer.h"
#include "../aslib/single/asSingle.h"

std::shared_ptr<uvTestServer> g_server = asSingle<uvTestServer>::instance();

int main()
{
	g_server->Init("0,0,0,0", 7890, 1024 * 10, 1024 * 10, 4,1000);
	g_server->TryRunNetWork(false);
	while (1)
	{

	}
	return 0;
}