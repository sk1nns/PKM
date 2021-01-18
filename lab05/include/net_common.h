#ifndef _NET_COMMON_
#define _NET_COMMON_
#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma pack(1)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string.h>
#endif // _WIN32
//Переводить рядкове представлення цільового хосту
//у числовий формат для Socket API
int resolve_host(const char* host, sockaddr_in* addr)
{
    memset(addr, 0, sizeof(struct sockaddr_in));
    unsigned int laddr = inet_addr(host);
    if (laddr == INADDR_NONE)
    {
        hostent* he = gethostbyname(host);
        if (he == NULL)
        {
            return -1;
        }
        addr->sin_family = he->h_addrtype;
        memcpy(&(addr->sin_addr), *(he->h_addr_list),
               he->h_length);
        return 0;
    }
    else
    {
        addr->sin_family = AF_INET;
        (addr->sin_addr).s_addr = laddr;
        return 0;
    }
}

#endif // _NET_COMMON_
