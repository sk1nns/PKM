#include "single_thread_udp_receiver.h"
#include <string>
using namespace std;

char caesar(char c);

unsigned int HashFAQ6(const char * str);

bool process_receive_data(SOCKET socket) {
	sockaddr_in incom_addr;
	memset(&incom_addr, 0, sizeof(incom_addr));
	socklen_t len = sizeof(incom_addr);

    while (true) {
        char buffer[256] = "";
        string output = "";
        int rc = recvfrom(socket, buffer, sizeof(buffer), 0, (sockaddr*)&incom_addr, &len);
        if (rc > 0) {
            printf("[%s]: Received message - %s\n", inet_ntoa(incom_addr.sin_addr), buffer);

            for (int x = 0; x < sizeof(buffer)/sizeof(int); x++)
            {
                output += caesar(buffer[x]);
            }

            printf("[%s]: Decoded message - %s\n", inet_ntoa(incom_addr.sin_addr), output.c_str());
        }
        else {
            break;
        }
    }

	return true;
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
