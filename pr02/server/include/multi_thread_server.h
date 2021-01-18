#ifndef NETWORK_PROGRAMMING_MULTI_THREAD_SERVER_H
#define NETWORK_PROGRAMMING_MULTI_THREAD_SERVER_H

#include "common_net.h"
#include "common_thread.h"

#include <vector>

#define DEFAULT_PORT 5557
#define CONNECTION_QUEUE 100

THREAD_RESULT handle_connection(void*);
#endif //NETWORK_PROGRAMMING_MULTI_THREAD_SERVER_H