#include "single_thread_udp_receiver.h"

void exit_handler();

SOCKET receiver_socket;

int main(int argc, char *argv[]) {	
    atexit(common_exit_handler);
	atexit(exit_handler);

    short port = DEFAULT_PORT;
    char host[128] = "";
    bool parse_cmd_result = parse_cmd(argc, argv, host, &port);

    common_init_handler();

    receiver_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (receiver_socket <= 0) {
        error_msg("Can't create socket");
        return -1;
    }

    sockaddr_in receiver_addr;
    init_inet_address(&receiver_addr, host, port);

	//Bind socket to the address on the server
	if (bind(receiver_socket, (sockaddr *)&receiver_addr, sizeof(sockaddr))) {
		char err_msg[128] = "";
		sprintf(err_msg, "Can't bind socket to the port %d", port);
		error_msg(err_msg);
		return -1;
	}

    printf("Receiver available on the port %d\n", port);

    while (process_receive_data(receiver_socket)) {}

    return 0;
}

void exit_handler() {
    close_socket(receiver_socket);
}