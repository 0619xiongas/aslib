#include "asNetBase.h"
#include <cstring>
bool InitSocketLib()
{
#if _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return false;
    }
#endif
    return true;
}

bool FreeSocketLib()
{
#if _WIN32
    WSACleanup();
#endif
    return true;
}

void CloseSocket(SOCKET &sock)
{
#if _WIN32
    closesocket(sock);
#else
    close(sock);
#endif
    sock = INVALID_SOCKET;
}

int StartListen(SOCKET &sock, const char *ip, unsigned short port)
{
    int ret = 0;
    SOCKADDR_IN saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(ip);
    saddr.sin_port = htons(port);
    ret = bind(sock, (SOCKADDR *)&saddr, sizeof(saddr));
    if(ret) return ret;
    ret = listen(sock, 0x7ffffff);
    if(ret) return ret;
    return 0;
}

int ConnectToServer(const char *ip, unsigned short port, SOCKET &sock)
{
    int ret = 0;
    SOCKADDR_IN saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(ip);
    saddr.sin_port = htons(port);
    ret = connect(sock, (SOCKADDR *)&saddr, sizeof(saddr));
    if(ret) return ret;
    return 0;
}

void SetSocketNonBlock(SOCKET &sock)
{
#if _WIN32
    u_long iMode = 1;
    ioctlsocket(sock, FIONBIO, &iMode);
#else
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);
#endif
}

bool SetSendBufferSize(SOCKET &sock, unsigned int size)
{
#if _WIN32
    int ret = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(size));
    if (ret == SOCKET_ERROR)
    {
        return false;
    }
#else
    int ret = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(size));
    if (ret == -1)
    {
        return false;
    }
#endif
    return true;
}

bool SetRecvBufferSize(SOCKET &sock, unsigned int size)
{
#if _WIN32
    int ret = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(size));
    if (ret == SOCKET_ERROR)
    {
        return false;
    }
#else
    int ret = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(size));
    if (ret == -1)
    {
        return false;
    }
#endif
    return true;
}
