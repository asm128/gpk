#define GPK_CONSOLE_LOG_ENABLED
#include "gpk_stdsocket.h"

int main() { 
	::gpk::tcpipInitialize();
	sockaddr_in				sa_server				= {};
	sa_server.sin_port	= htons(80);
	::gpk::tcpipAddressToSockaddr({{}, 80}, sa_server);

	SOCKET					handle					= socket(AF_INET, SOCK_DGRAM, 0);
	ree_if(INVALID_SOCKET == handle, "Failed to create socket.");
	gpk_necall(::bind(handle, (sockaddr *)&sa_server, sizeof(sockaddr_in)), "Failed to bind listener to address");
	while(true) {
		sockaddr_in				sa_client				= {};
		int						client_length			= sizeof(sa_client);
		char					connectReceived			= 0;
		gpk_necall(::recvfrom(handle, (char*)&connectReceived, (int)sizeof(char), 0, (sockaddr*)&sa_client, &client_length), "Failed to receive connect request.");
		::gpk::SIPv4			addrRemote;
		::gpk::tcpipAddressFromSockaddr(sa_client, addrRemote);
		info_printf("Received connect request: %c from %u.%u.%u.%u:%u.", connectReceived, GPK_IPV4_EXPAND(addrRemote));

		char					commandToSend			= '2';
		gpk_necall(::sendto(handle, (const char*)&commandToSend, (int)sizeof(char), 0, (sockaddr*)&sa_client, sizeof(sockaddr_in)), "Failed to respond.");
		info_printf("Sending connect response: %c", connectReceived);
	}
	::gpk::tcpipShutdown();
	return 0; 
}
