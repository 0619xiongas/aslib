#ifndef AS_NETBASE_H
#define AS_NETBASE_H

/*
    用于socket网络库的基础定义参数以及不同平台下的对于socket的api封装
    windows : iocp
    linux : epoll
*/
#ifdef _WIN32
#include <WinSock2.h>
#include <ws2ipdef.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <MSWSock.h>
#else
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
typedef int SOCKET;
typedef sockaddr SOCKADDR;
typedef sockaddr_in SOCKADDR_IN;
#endif

bool InitSocketLib();
bool FreeSocketLib();
void CloseSocket(SOCKET& sock);
int StartListen(SOCKET& sock, const char* ip, unsigned short port);
int ConnectToServer(const char* ip, unsigned short port, SOCKET& sock);
void SetSocketNonBlock(SOCKET& sock);
bool SetSendBufferSize(SOCKET& sock, unsigned int size);
bool SetRecvBufferSize(SOCKET& sock, unsigned int size);
#endif // AS_NETBASE_H