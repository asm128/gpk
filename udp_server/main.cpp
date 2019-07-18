#include "gpk_stdsocket.h"

int main() { 
	::gpk::tcpipInitialize();
	sockaddr_in			sa_server			= {};
	sockaddr_in			sa_client			= {};
	int					client_length		= sizeof(sa_client);
	sa_server.sin_port	= htons(80);
	::gpk::tcpipAddressToSockaddr({{}, 80}, sa_server);


	// 1. Server:
	SOCKET				handle				= socket(AF_INET, SOCK_DGRAM, 0);
	char				connectReceived		= 0;
	::bind(handle, (sockaddr *)&sa_server, sizeof(sockaddr_in));
	while(true) {
		gpk_necall(::recvfrom(handle, (char*)&connectReceived, (int)sizeof(char), 0, (sockaddr*)&sa_client, &client_length), "Failed to receive connect request.");
		info_printf("Received connect request: %c", connectReceived);

		// 3. Server:
		char				commandToSend		= '2';
		gpk_necall(::sendto(handle, (const char*)&commandToSend, (int)sizeof(char), 0, (sockaddr*)&sa_client, sizeof(sockaddr_in)), "Failed to respond.");
		info_printf("Sending connect response: %c", connectReceived);
	}
	::gpk::tcpipShutdown();
	return 0; 
}
