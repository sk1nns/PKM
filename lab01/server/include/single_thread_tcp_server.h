#ifndef NETWORK_PROGRAMMING_SINGLE_THREAD_TCP_SERVER_H
#define NETWORK_PROGRAMMING_SINGLE_THREAD_TCP_SERVER_H

#include "common_net.h"
#include "common_utils.h"

#define DEFAULT_PORT 5557
#define CONNECTION_QUEUE 100

void handle_connection(SOCKET, sockaddr_in*);
#endif //NETWORK_PROGRAMMING_SINGLE_THREAD_TCP_SERVER_H
