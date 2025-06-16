#include <iostream>
#include "uvTestServer.h"
#include "single/asSingle.h"

std::shared_ptr<uvTestServer> g_server = asSingle<uvTestServer>::instance();

int main()
{
	g_server->Init("0,0,0,0", 10011, 1024 * 10, 1024 * 10, 4,1000);
	std::cout << std::endl;
	g_server->TryRunNetWork(false);
	while (1)
	{

	}
	return 0;
}