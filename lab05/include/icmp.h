#include <windows.h>
#include <stdio.h>
#include <string.h>

#ifndef _ICMP_
#define _ICMP_
#define IP_RECORD_ROUTE 0x7
#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0
#define ICMP_MIN 8 // Пакет ICMP не менше 8 байт (заголовок)
#define DEFAULT_PACKET_SIZE 32 // Стандартний розмір пакету
#define MAX_PACKET_SIZE 1024 // Максимальний розмір ICMP -пакета
#define MAX_IP_HDR_SIZE 60 // Максимальний розмір
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long ulong_t;
// Структура IP-заголовка
typedef struct _iphdr
{
    unsigned int h_len : 4; // Довжина заголовка
    unsigned int version : 4; // Версія IP
    uint8_t tos; // Тип служби
    uint16_t total_len; // Повний розмір пакету
    uint16_t ident; // Унікальний ідентифікатор
    uint16_t flags; // Прапори
    uint8_t ttl; // Час життя
    uint8_t proto; // Протокол (TCP, UDP і тому подібне)
    uint16_t checksum; // Контрольна сума IP
    ulong_t sourceIP;
    ulong_t destIP;
} IP_HEADER, *PIP_HEADER;
// Структура заголовка ICMP
typedef struct _icmphdr
{
    uint8_t type;
    uint8_t code; // Тип субкоду
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;
    ulong_t timestamp;
} ICMP_HEADER, *PICMP_HEADER;
// Розширений заголовок IP -
// використовується з параметром IP_OPTIONS
typedef struct _ipoptionhdr
{
    uint8_t code; // Тип параметра
    uint8_t len; // Довжина розширеного заголовка
    uint8_t ptr; // Зміщення першої адреси
    ulong_t addr[9]; // Перелік IP адрес
} IP_OPTION_HEADER, *PIP_OPTION_HEADER;
// Допоміжна функція заповнення полів ICMP-запиту
void fill_icmp_data(char *icmp_data, int datasize) {
    PICMP_HEADER picmp_hdr = (PICMP_HEADER) icmp_data;
    picmp_hdr = (PICMP_HEADER) icmp_data;
    picmp_hdr->type = ICMP_ECHO; // Луна-запит ICMP
    picmp_hdr->code = 0;
    picmp_hdr->id = (uint16_t) GetCurrentProcessId();
    picmp_hdr->checksum = 0;
    picmp_hdr->seq = 0;
    char *datapart = icmp_data + sizeof(ICMP_HEADER);
// Помістимо які-небудь дані у буфер
    memset(datapart, 'E', datasize - sizeof(ICMP_HEADER));
}

// Обчислює 16-бітову суму комплементу
// для вказаного буфера із заголовком
uint16_t checksum(uint16_t *buffer, int size) {
    ulong_t cksum = 0;
    while (size > 1) {
        cksum += *buffer++;
        size -= sizeof(uint16_t);
    }
    if (size) {
        cksum += *(uint16_t *) buffer;
    }
    cksum = (cksum & 0xffff) + (cksum >> 16);
    cksum += (cksum >> 16);
    return (uint16_t) ~cksum;
}
// Якщо є розширений заголовок, знаходить в ньому
// параметри IP і виводить значення параметра запису маршруту
void decode_ip_opts(char *buf, int bytes) {
    PIP_OPTION_HEADER ipopt = (PIP_OPTION_HEADER) (buf + 20);
    in_addr inaddr;
    hostent *host = NULL;
    printf("RR: ");
    int i, n = ipopt->ptr / 4;
    for (i = 0; i < n - 1; ++i) {
        char delim = i + 1 < n - 1 ? '\t' : '\n';
        inaddr.S_un.S_addr = ipopt->addr[i];
        host = gethostbyaddr((char *) &inaddr.S_un.S_addr,
                             sizeof(inaddr.S_un.S_addr), AF_INET);
        if (host) {
            printf("(%-15s) %s", inet_ntoa(inaddr), host->h_name);
        } else {
            printf("(%-15s)", inet_ntoa(inaddr));
        }
        printf("%c", delim);
    }
}
// Декодує IP - заголовок пакету у відповідь
// для знаходження даних ICMP.
void decode_icmp_hdr(char *buf, int bytes,
                     struct sockaddr_in *from) {
    static int icmpcount = 0;
    PIP_HEADER iphdr = (PIP_HEADER) buf;
    uint16_t iphdrlen = iphdr->h_len * 4;
    PICMP_HEADER icmphdr = (PICMP_HEADER) (buf + iphdrlen);
    char *str_from = inet_ntoa(from->sin_addr);
    ulong_t tick =
#ifdef _WIN32
            GetTickCount();
#endif // _WIN32
    if (iphdrlen == MAX_IP_HDR_SIZE && !icmpcount) {
        decode_ip_opts(buf, bytes);
    }
    if (bytes < iphdrlen + ICMP_MIN) {
        printf("Too few bytes from %s\n", str_from);
    }
    if (icmphdr->type != ICMP_ECHOREPLY) {
        printf("non-echo type %d reponse\n", icmphdr->type);
        return;
    }
// Перевірка, що це ICMP-відповідь на повідомлення
    uint16_t pid =
#ifdef _WIN32
            (uint16_t) GetCurrentProcessId();
#endif // _WIN32
    if (icmphdr->id != pid) {
        printf("someone else responded packet!\n");
        return;
    }
    printf("\tReceived %d bytes from %s:", bytes, str_from);
    printf("\tICMP_seq = %d", icmphdr->seq);
    printf("\tTime: %d ms\n", tick - icmphdr->timestamp);
    icmpcount++;
}
#endif //_ICMP_
