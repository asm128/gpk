#define GPK_STDOUT_LOG_ENABLED
#include "gpk_stdsocket.h"
#include "gpk_sync.h"
#include "gpk_runtime.h"

using namespace gpk;

static ::gpk::err_t udpServerMain(::gpk::SRuntimeValues &) {
	tcpipInitialize();
	sockaddr_in				sa_server				= {};
	SIPv4Endpoint			addrLocal				= {{}, 9898};
	tcpipAddress({}, 9898, 1, TRANSPORT_PROTOCOL_UDP, addrLocal);
	tcpipAddressToSockaddr(addrLocal, sa_server);

	SOCKET					handle					= socket(AF_INET, SOCK_DGRAM, 0);
	if_true_fe(INVALID_SOCKET == handle);
	if_fail_fef(::bind(handle, (sockaddr *)&sa_server, sizeof(sockaddr_in)), "Failed to bind listener to address %u", (u2_t)addrLocal);
	info_printf("Server listening on %u.%u.%u.%u:%u.", GPK_IPV4_EXPAND(addrLocal));
	while(true) {
		SIPv4Endpoint			addrLocalClient			= addrLocal;
		addrLocalClient.Port = 0;
		SOCKET					clientHandle			= socket(AF_INET, SOCK_DGRAM, 0);

		sockaddr_in				sa_server_client		= {AF_INET};
		tcpipAddressToSockaddr(addrLocalClient, sa_server_client);
		if_fail_fef(::bind(clientHandle, (sockaddr *)&sa_server_client, sizeof(sockaddr_in)), "Failed to bind listener to address %u", (u2_t)addrLocalClient);

		sockaddr_in				sa_client				= {AF_INET};
		int						client_length			= sizeof(sa_client);
		char					connectReceived			= 0;
		if_fail_fef(::recvfrom(handle, (char*)&connectReceived, (int)sizeof(char), 0, (sockaddr*)&sa_client, &client_length), "Failed to receive connect request from address %u", (u2_t)addrLocalClient);
		SIPv4Endpoint			addrRemote;
		tcpipAddressFromSockaddr(sa_client, addrRemote);
		info_printf("Received connect request: %c from %u.%u.%u.%u:%u.", connectReceived, GPK_IPV4_EXPAND(addrRemote));

		char					commandToSend			= '2';
		//tcpipAddressFromSockaddr(sa_server, addrLocal);
		tcpipAddress(clientHandle, addrLocal);
		info_printf("Sending connect response %c from %u.%u.%u.%u:%u to %u.%u.%u.%u:%u.", commandToSend, GPK_IPV4_EXPAND(addrLocal), GPK_IPV4_EXPAND(addrRemote));
		sleep(10);
		if_true_fef(INVALID_SOCKET == clientHandle, "Failed to create socket for address %u", (u2_t)addrLocal);
		while(true) {
			if_fail_fef(::sendto(clientHandle, (const char*)&commandToSend, (int)sizeof(char), 0, (sockaddr*)&sa_client, sizeof(sockaddr_in)), "Failed to respond address %u", (u2_t)addrRemote);
			sleep(10);
		}
		info_printf("Sent connect response %c from %u.%u.%u.%u:%u to %u.%u.%u.%u:%u.", commandToSend, GPK_IPV4_EXPAND(addrLocal), GPK_IPV4_EXPAND(addrRemote));
		if(handle != clientHandle) {
			if(clientHandle!= INVALID_SOCKET)
				closesocket(clientHandle);
		}
	}
	tcpipShutdown();
	return 0;
}

GPK_OS_ENTRY_POINT(::udpServerMain);
