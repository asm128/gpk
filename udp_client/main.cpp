#include "gpk_stdsocket.h"
#include "gpk_sync.h"

int main() {
	::gpk::tcpipInitialize();
	sockaddr_in			sa_server			= {};
	::gpk::tcpipAddressToSockaddr({{192,168,0,2}, 80}, sa_server);
	while(true) {
		SOCKET				handle				= socket(AF_INET, SOCK_DGRAM, 0);
		ree_if(INVALID_SOCKET == handle, "Failed to create socket.");
		char				commandToSend		= '1';
		int					sa_length			= sizeof(sa_server);
		gpk_necall(sendto(handle, (const char*)&commandToSend, (int)sizeof(char), 0, (sockaddr *)&sa_server, sa_length), "Failed to send connect request to server.");
		info_printf("Send connect request to server: %c.", commandToSend);

		char				commandReceived		= 0;
		gpk_necall(recvfrom(handle, (char *)&commandReceived, (int)sizeof(char), 0, (sockaddr *)&sa_server, &sa_length), "Failed to receive response from server");
		info_printf("Received connect response from server: %c.", commandReceived);
		::gpk::sleep(100);
		gpk_safe_closesocket(handle);
	}
	::gpk::tcpipShutdown();
	return 0;
}
