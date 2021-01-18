#include "single_thread_tcp_server.h"

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

void handle_connection(SOCKET socket, sockaddr_in* addr) {
    char* str_in_addr = inet_ntoa(addr->sin_addr);
    printf("[%s]>>%s\n", str_in_addr, "Establish new connection");
    while (true) {
        char buffer[256] = "";
        char message[512] = "";
        int keynumb = rand() % 10;
        char key[10] = "";
        int rc = recv(socket, buffer, sizeof(buffer), 0);
        if (rc > 0) {
            printf("[%s]:%s\n", str_in_addr, buffer);
            strcat(message, ceasarEncryption(buffer, keynumb));
            printf("Encoded message: ");
            printf(message);
            printf("\n");
            sprintf(key, "\nkey: %d", keynumb);
            strcat(message,key);
            int iSendResult = send(socket, message, sizeof(message), 0);
        }
        else {
            break;
        }
    }
    closesocket(socket);
    //printf("[%s]>>%s", str_in_addr, "Close incomming connection");
}