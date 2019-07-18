#define GPK_CONSOLE_LOG_ENABLED
#include "gpk_stdsocket.h"

int main() { 
	::gpk::tcpipInitialize();
	sockaddr_in				sa_server				= {};
	::gpk::SIPv4			addrLocal				= {{}, 9898};
	::gpk::tcpipAddress(0, 9898, 1, ::gpk::TRANSPORT_PROTOCOL_UDP, addrLocal);
	::gpk::tcpipAddressToSockaddr(addrLocal, sa_server);

	SOCKET					handle					= socket(AF_INET, SOCK_DGRAM, 0);
	ree_if(INVALID_SOCKET == handle, "Failed to create socket.");
	gpk_necall(::bind(handle, (sockaddr *)&sa_server, sizeof(sockaddr_in)), "Failed to bind listener to address");
	info_printf("Server listening on %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(addrLocal));
	while(true) {
		sockaddr_in				sa_client				= {AF_INET};
		int						client_length			= sizeof(sa_client);
		char					connectReceived			= 0;
		gpk_necall(::recvfrom(handle, (char*)&connectReceived, (int)sizeof(char), 0, (sockaddr*)&sa_client, &client_length), "Failed to receive connect request.");
		::gpk::SIPv4			addrRemote;
		::gpk::tcpipAddressFromSockaddr(sa_client, addrRemote);
		info_printf("Received connect request: %c from %u.%u.%u.%u:%u.", connectReceived, GPK_IPV4_EXPAND(addrRemote));
		
		char					commandToSend			= '2';
		//::gpk::tcpipAddressFromSockaddr(sa_server, addrLocal);
		addrLocal.Port = 0;
		info_printf("Sending connect response %c from %u.%u.%u.%u:%u to %u.%u.%u.%u:%u.", commandToSend, GPK_IPV4_EXPAND(addrLocal), GPK_IPV4_EXPAND(addrRemote));

		SOCKET					clientHandle			= socket(AF_INET, SOCK_DGRAM, 0);

		sockaddr_in				sa_server_client		= {AF_INET};
		::gpk::tcpipAddressToSockaddr(addrLocal, sa_server_client);

		gpk_necall(::bind(clientHandle, (sockaddr *)&sa_server_client, sizeof(sockaddr_in)), "Failed to bind listener to address");
		::gpk::tcpipAddress(clientHandle, addrLocal);
		ree_if(INVALID_SOCKET == clientHandle, "Failed to create socket.");
		gpk_necall(::sendto(clientHandle, (const char*)&commandToSend, (int)sizeof(char), 0, (sockaddr*)&sa_client, sizeof(sockaddr_in)), "Failed to respond.");
		info_printf("Sent connect response %c from %u.%u.%u.%u:%u to %u.%u.%u.%u:%u.", commandToSend, GPK_IPV4_EXPAND(addrLocal), GPK_IPV4_EXPAND(addrRemote));
		if(handle != clientHandle)
			gpk_safe_closesocket(clientHandle);
	}
	::gpk::tcpipShutdown();
	return 0; 
}
