#include <iostream>
#include "ucTestClient.h"
#include "../aslib/single/asSingle.h"
#include <memory>
#include <chrono>
#include <thread>

std::shared_ptr<uvTestClient> g_client = asSingle<uvTestClient>::instance();

void SendData(u32 sessionId,u32 msgId, char* buf, u32 len)
{
	u32 sendLen = sizeof(asNetTcpMsgHead);
	if (buf)
	{
		sendLen += len;
	}
	asNetTcpMsgHead head;
	head.m_msgId = msgId;
	head.m_len = sendLen;
	char* data = new char[sendLen];
	::memcpy(data, &head, sizeof(asNetTcpMsgHead));
	::memcpy(data + sizeof(asNetTcpMsgHead), buf, len);
	g_client->DoSendData(data, sendLen, sessionId);
}
std::string info = "this is uv net work test,send to server!!!";

int main()
{
	g_client->Init("127.0.0.1", 7890, 1024 * 10, 1024 * 10, 1, 100);
	g_client->TryRunNetWork(true);
	while (true) {
		auto start = std::chrono::steady_clock::now();

		// 打印信息
		u32 sid = g_client->GetServerSession();
		if (sid != 0)
		{
			std::cout << "Client is send data" << std::endl;
			SendData(g_client->GetServerSession(), 1, (char*)info.c_str(), info.length());
		}

		// 计算剩余等待时间（确保精确3秒间隔）
		auto end = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (elapsed < 3000) {
			std::this_thread::sleep_for(std::chrono::milliseconds(3000 - elapsed));
		}
	}
	return 0;
}