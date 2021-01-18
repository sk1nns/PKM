#include <stdio.h>
#include <stdlib.h>
#include "net_common.h"
#include "icmp.h"
#include "ping_hlp.h"

// виводить інформацію про використання
void usage(char *);

// обробник завершення
void exit_handler();
//Макрос для перевірки встановлення опцій сокету
#define CHECK_SOCK_OPT(ret, option)\
if (ret == SOCKET_ERROR)\
{\
    printf("setsockopt(%s) failed: %d\n", option,WSAGetLastError());\
    return -1;\
}
//Макрос для перевірки результату введення-виведення
#define CHECK_SOCK_TRANSMIT(ret, transmit)\
if (ret == SOCKET_ERROR)\
{\
    if (WSAGetLastError() == WSAETIMEDOUT)\
    {\
        printf("timed out\n");\
        continue;\
    }\
    printf("%s failed: %d\n", transmit, WSAGetLastError());\
    return -1;\
}
//Макрос для перевірки виділення пам'яті
#define CHECK_ALLOC_MEM(buffer, msg)\
if (!buffer)\
{\
    printf("Error allocate memory for %s\n", msg);\
    return -1;\
}
//Глобальні змінні сокету та буферів даних
SOCKET sock_raw = INVALID_SOCKET;
char *icmp_data = NULL, *recvbuf = NULL;

// Настроює простий сокет ICMP, створює заголовок ICMP.
// Додає розширений заголовок IP,
// розсилає ICMP луну-запити по кінцевих точках.
// Задає тайм-аут в уникнення
// простою. Прийнятий пакет декодується.
int main(int argc, char **argv) {
    atexit(exit_handler);
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup() failed: %d", GetLastError());
        return -1;
    }
    char host[256] = "";
    PING_CMD_OPTIONS cmd_opts;
    SET_DEF_CMD_OPTS(cmd_opts);
    if (parse_cmd(argc, argv, host, &cmd_opts)) {
        usage(argv[0]);
        return -1;
    }
    printf("Ping host: %s\n", host);
    struct sockaddr_in dest, from;
    int ret;
    if (resolve_host(host, &dest)) {
        printf("Can't resolve host %s\n", host);
        return -1;
    }
    printf("Resolved IP address: %s\n",
           inet_ntoa(dest.sin_addr));
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock_raw == INVALID_SOCKET) {
        printf("socket() failed: %d\n", WSAGetLastError());
        return -1;
    }
    IP_OPTION_HEADER ipopt;
    if (cmd_opts.record_route) {
// Налаштування відправки розширеного заголовка IP
// з кожним ICMP пакетом
        memset(&ipopt, 0, sizeof(ipopt));
        ipopt.code = IP_RECORD_ROUTE; // Запис маршруту
        ipopt.ptr = 4; // Зміщення першої адреси
        ipopt.len = 39; // Довжина розширеного заголовка
        ret = setsockopt(sock_raw, IPPROTO_IP, IP_OPTIONS,
                         (char *) &ipopt, sizeof(ipopt));
        CHECK_SOCK_OPT(ret, "IP_OPTIONS");
    }
// Налаштування тайм-аутів відправки і прийому
    ret = setsockopt(sock_raw, SOL_SOCKET, SO_RCVTIMEO,
                         (char *) &(cmd_opts.timeout),
                         sizeof(cmd_opts.timeout));
    CHECK_SOCK_OPT(ret, "SO_RCVTIMEO");
    ret = setsockopt(sock_raw, SOL_SOCKET, SO_SNDTIMEO,
                             (char *) &(cmd_opts.timeout),
                             sizeof(cmd_opts.timeout));
    CHECK_SOCK_OPT(ret, "SO_SNDTIMEO");
                // Створення ICMP -пакета
    cmd_opts.packet_size += sizeof(ICMP_HEADER);
    icmp_data = (char *) malloc(MAX_PACKET_SIZE);
    CHECK_ALLOC_MEM(icmp_data, "ICMP packet");
    memset(icmp_data, 0, MAX_PACKET_SIZE);
    fill_icmp_data(icmp_data, cmd_opts.packet_size);
    recvbuf = (char *) malloc(MAX_PACKET_SIZE);
    CHECK_ALLOC_MEM(recvbuf, "receive buffer");
    int i;
    uint16_t seq_no = 0;
// Початок відправки/прийому ICMP -пакетов
    for (i = 0; i < cmd_opts.ping_count; ++i) {
        PICMP_HEADER icmp_hdr = (PICMP_HEADER) icmp_data;
        icmp_hdr->checksum = 0;
        icmp_hdr->timestamp = GetTickCount();
        icmp_hdr->seq = seq_no++;
        icmp_hdr->checksum = checksum((uint16_t *) icmp_data, cmd_opts.packet_size);
        ret = sendto(sock_raw, icmp_data,
                                 cmd_opts.packet_size, 0,
                                 (struct sockaddr *) &dest, sizeof(dest));
        CHECK_SOCK_TRANSMIT(ret, "sendto()");
        if (ret < cmd_opts.packet_size) {
            printf("Wrote %d bytes\n", ret);
        }
        int from_len = sizeof(sockaddr_in);
        ret = recvfrom(sock_raw, recvbuf, MAX_PACKET_SIZE, 0,
                                   (struct sockaddr *) &from, &from_len);
        CHECK_SOCK_TRANSMIT(ret, "recvfrom()");
        decode_icmp_hdr(recvbuf, ret, &from);
#ifdef _WIN32
        Sleep(1000);
#endif // _WIN32
    }

    return 0;
}
void usage(char *exename) {
    printf("usage: %s [OPTIONS] <host>\n", exename);
    printf(" OPTIONS can be one from the following:\n");
    printf("\t -sz:<packet size> - size of packet\n");
    printf("\t -r - route recording\n");
    printf("\t -t:<timeout> - ping timeout\n");
    printf("\t -cn:<count> - ping count\n");
}

void exit_handler() {
    if (WSACleanup()) {
        printf("Error WinSock cleanup: %d\n", WSAGetLastError());
    }
    if (sock_raw != INVALID_SOCKET) {
        closesocket(sock_raw);
    }
    if (icmp_data) {
        free(icmp_data);
    }
    if (recvbuf) {
        free(recvbuf);
    }
}