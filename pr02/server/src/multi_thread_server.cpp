#include "multi_thread_server.h"

char* ceasarEncryption(char* strng, int key)
{
    char ch;
    for (int i = 0; strng[i] != '\0'; ++i) {
        ch = strng[i];
        //encrypt for lowercase letter
        if (ch >= 'a' && ch <= 'z') {
            ch = ch + key;
            if (ch > 'z') {
                ch = ch - 'z' + 'a' - 1;
            }
            strng[i] = ch;
        }
        //encrypt for uppercase letter
        else if (ch >= 'A' && ch <= 'Z') {
            ch = ch + key;
            if (ch > 'Z') {
                ch = ch - 'Z' + 'A' - 1;
            }
            strng[i] = ch;
        }
    }
    //printf("Encrypted message: %s", strng);
    return strng;
}

THREAD_RESULT handle_connection(void* psocket) {
	CHECK_IO(psocket, -1, "Empty connection thread data\n");
	SOCKET socket;
	CHECK_IO((socket = *((SOCKET*)psocket)) > 0, -1, "Invalid connection thread data\n");
	sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	CHECK_IO(!getpeername(socket, (sockaddr*)&addr, &addr_len), -1, "Error retrieving peer info\n");
	char* str_in_addr = inet_ntoa(addr.sin_addr);
	printf("[%s:%d]>>%s\n", str_in_addr, ntohs(addr.sin_port), "Establish new connection");
	while (true) {
        char buffer[256] = "";
        char message[512] = "";
        int keynumb = rand() % 10;
        char key[10] = "";
		int rc = recv(socket, buffer, sizeof(buffer), 0);
		printf("%s\n",buffer);
		if (rc > 0) {
            printf("[%s]:%s\n", str_in_addr, buffer);
            strcat(message, ceasarEncryption(buffer, keynumb));
            printf("Encoded message: ");
            printf(message);
            printf("\n");
            sprintf(key, "\nkey: %d", keynumb);
            strcat(message, key);
            int iSendResult = send(socket, message, sizeof(message), 0);
        }
	close_socket(socket);
	printf("[%s]>>%s", str_in_addr, "Close incomming connection");
	return 0;
}
}