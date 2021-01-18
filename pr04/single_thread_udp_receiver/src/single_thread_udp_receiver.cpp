#include "single_thread_udp_receiver.h"

bool process_receive_data(SOCKET socket) {
	sockaddr_in incom_addr;
	memset(&incom_addr, 0, sizeof(incom_addr));
	socklen_t len = sizeof(incom_addr);
	char buffer[256] = "";
	int rec_cn = recvfrom(socket, buffer, sizeof(buffer), 0, (sockaddr*)&incom_addr, &len);

	if (rec_cn <= 0) {
		error_msg("Can't receive data");		
	}

	printf("[From: %s] Received data [%s]\n", inet_ntoa(incom_addr.sin_addr), buffer);

	return true;
}