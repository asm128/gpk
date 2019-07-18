#define GPK_CONSOLE_LOG_ENABLED
#include "gpk_stdsocket.h"
#include "gpk_sync.h"

int main() {
	::gpk::tcpipInitialize();
	sockaddr_in			sa_server			= {AF_INET};
	while(true) {
//#define MAKE_IT_WORK
#if defined MAKE_IT_WORK
		::gpk::tcpipAddressToSockaddr({{192,168,0,2}, 9898}, sa_server);
#else
		::gpk::tcpipAddressToSockaddr({{201,235,131,233}, 9898}, sa_server);
#endif
		::gpk::SIPv4		addrRemote			= {};
		SOCKET				handle				= socket(AF_INET, SOCK_DGRAM, 0);
		ree_if(INVALID_SOCKET == handle, "Failed to create socket.");
		sockaddr_in			sa_client			= {AF_INET};
		gpk_necall(::bind(handle, (sockaddr *)&sa_client, sizeof(sockaddr_in)), "Failed to bind listener to address");
		char				commandToSend		= '1';
		int					sa_length			= sizeof(sa_server);
		gpk_necall(sendto(handle, (const char*)&commandToSend, (int)sizeof(char), 0, (sockaddr *)&sa_server, sa_length), "Failed to send connect request to server.");
		{
			sockaddr_in			sa_battery			= sa_server;
			for(uint32_t i=16*1024; i < 64*1024; ++i) {
				sa_battery.sin_port	= htons((u_short)i);
				gpk_necall(sendto(handle, (const char*)&commandToSend, (int)sizeof(char), 0, (sockaddr *)&sa_battery, sa_length), "Failed to send connect request to server.");
				//::gpk::sleep(1);
			}
		}
		::gpk::tcpipAddressFromSockaddr(sa_server, addrRemote);
		info_printf("Send connect request to server: %c to %u.%u.%u.%u:%u", commandToSend, GPK_IPV4_EXPAND(addrRemote));

		char				connectAcknowledge	= 0;
		sa_server.sin_port	= 0;
		gpk_necall(recvfrom(handle, (char *)&connectAcknowledge, (int)sizeof(char), 0, (sockaddr *)&sa_server, &sa_length), "Failed to receive response from server");
		addrRemote			= {};
		::gpk::tcpipAddressFromSockaddr(sa_server, addrRemote);
		info_printf("Received connect response from server: %c from %u.%u.%u.%u:%u.", connectAcknowledge, GPK_IPV4_EXPAND(addrRemote));
		::gpk::sleep(1000);
		gpk_safe_closesocket(handle);
	}
	::gpk::tcpipShutdown();
	return 0;
}
