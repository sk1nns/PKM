#include "ping.h"
#include <string>
using namespace std;

char caesar(char c);

THREAD_VOID handle_connection(void* psocket) {
	CHECK_VOID_IO(psocket, "Empty connection thread data\n");
	SOCKET socket;
	CHECK_VOID_IO((socket = ((SOCKET)psocket)) > 0, "Invalid connection thread data\n");
	sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	CHECK_VOID_IO(!getpeername(socket, (sockaddr*)&addr, &addr_len), "Error retrieving peer info\n");
	char* str_in_addr = inet_ntoa(addr.sin_addr);
	printf("[%s:%d]>>%s\n", str_in_addr, ntohs(addr.sin_port), "Establish new connection");

    while (true) {
        char buffer[256] = "";
        string output = "";
        int rc = recv(socket, buffer, sizeof(buffer), 0);
        if (rc > 0) {
            printf("[%s]: Received message - %s\n", str_in_addr, buffer);

            for (int x = 0; x < sizeof(buffer)/sizeof(int); x++)
            {
                output += caesar(buffer[x]);
            }

            printf("[%s]: Decoded message - %s\n", str_in_addr, output.c_str());
        }
        else {
            break;
        }
    }

	close_socket(socket);
	printf("[%s:%d]>>%s\n", str_in_addr, ntohs(addr.sin_port), "Close incoming connection");
}

char caesar(char c)
{
    if (isalpha(c))
    {
        c = toupper(c);
        c = (((c - 65) + 13) % 26) + 65;
    }
    return c;
}