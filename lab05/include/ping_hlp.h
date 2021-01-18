#ifndef _PING_HLP_
#define _PING_HLP_
#include <string.h>
#include "icmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

//Структура для опцій командного рядка
typedef struct _ping_cmd_opts {
    uint32_t packet_size;
    int record_route;
    int timeout;
    int ping_count;
} PING_CMD_OPTIONS, *PPING_CMD_OPTIONS;
#define SET_DEF_CMD_OPTS(cmd_opts)\
{\
 memset(&cmd_opts, 0, sizeof(cmd_opts));\
 cmd_opts.packet_size = DEFAULT_PACKET_SIZE;\
 cmd_opts.record_route = 0;\
 cmd_opts.timeout = 1000;\
 cmd_opts.ping_count = 4;\
}
//Коди помилок парсингу
#define EPARSE_CMD_COUNT -2
#define EPARSE_CMD_ANY -1
#define EPARSE_CMD_OK 0
//Парсинг опцій запуску ping
//Опції представлені через пробіл
void parse_cmd_opts(const char* opt, PPING_CMD_OPTIONS opts) {
    if (!strcmp(opt, "-r")) {
        opts->record_route = 1;
    } else {
        const char *p = strchr(opt, ':');
        if (p && strlen(p + 1)) {
            if (!strncmp(opt, "-sz", 3)) {
                opts->packet_size = atoi(p + 1);
            } else if (!strncmp(opt, "-t", 2)) {
                opts->timeout = atoi(p + 1);
            } else if (!strncmp(opt, "-cn", 3)) {
                opts->ping_count = atoi(p + 1);
            }
        }
    }
}
//Парсинг усіх параметрів командного рядка
int parse_cmd(int argc, char* argv[], char* dest,
              PPING_CMD_OPTIONS opts)
{
    if (argc < 2) {
        return EPARSE_CMD_COUNT;
    }
    int i;
    for (i = 1; i < argc - 1; ++i) {
        parse_cmd_opts(argv[i], opts);
    }
    sprintf(dest, "%s", argv[argc - 1]);
    return EPARSE_CMD_OK;
}
#endif //_PING_HLP_