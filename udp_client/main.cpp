#include "gpk_stdsocket.h"
#include "gpk_sync.h"

using namespace gpk;

int main() {
	::gpk::tcpipInitialize();
	sockaddr_in			sa_server			= {AF_INET};
	while(true) {
#define MAKE_IT_WORK
#if defined MAKE_IT_WORK
		::gpk::tcpipAddressToSockaddr({{192,168,100,59}, 9898}, sa_server);
#else
		::gpk::tcpipAddressToSockaddr({{201,235,131,233}, 9898}, sa_server);
#endif
		::gpk::SIPv4Endpoint		addrRemote			= {};
		SOCKET				handle				= socket(AF_INET, SOCK_DGRAM, 0);
#if defined GPK_WINDOWS
		u_long				iMode				= 1;
		int					iResult				;
		if_fail_fef(iResult = ioctlsocket(handle, FIONBIO, &iMode), "could not set file flags: %i.", iResult);
#else
		int					flags				;
		if_fail_fe(flags = fcntl(socket_fd, F_GETFL), "could not set file flags: %i.", flags);
		if_fail_fe(fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK));
#endif
		if_true_fe(INVALID_SOCKET == handle);
		sockaddr_in			sa_client			= {AF_INET};
		if_fail_fe(::bind(handle, (sockaddr *)&sa_client, sizeof(sockaddr_in)));
		char				commandToSend		= '1';
		int					sa_length			= sizeof(sa_server);
		if_fail_fe(::sendto(handle, (const char*)&commandToSend, (int)sizeof(char), 0, (sockaddr *)&sa_server, sa_length));
		::gpk::tcpipAddressFromSockaddr(sa_server, addrRemote);
		info_printf("Sent connect request to server: %c to %u.%u.%u.%u:%u", commandToSend, GPK_IPV4_EXPAND(addrRemote));

		char				connectAcknowledge	= 0;
		sa_server.sin_port	= 0;
		while(true) {
			connectAcknowledge	= 0;
			if_true_fef(failed(::recvfrom(handle, (char*)&connectAcknowledge, 1, MSG_PEEK, (sockaddr *)&sa_server, &sa_length))
				&& WSAGetLastError() != WSAEMSGSIZE
				&& WSAGetLastError() != WSAEWOULDBLOCK
				, "WSAGetLastError(): 0x%X(%i)"
				, GPKREP1(WSAGetLastError())
				);

			if('2' == connectAcknowledge) {
				info_printf("Connected!");
				break;
			}
			else {
				//::gpk::_gpk_print_system_errors("1", 1);
				sockaddr_in			sa_battery			= sa_server;
				for(uint32_t j=0; j < 10; ++j) {
				for(uint32_t i=0; i < 64*1024; ++i) {
					sa_battery.sin_port	= htons((u_short)i);
					if_fail_fef(::sendto(handle, (const char*)&commandToSend, (int)sizeof(char), 0, (sockaddr *)&sa_battery, sa_length), "Failed to send connect request to server.");
					::gpk::sleep(1);
			}}
		}

		}
		addrRemote			= {};
		::gpk::tcpipAddressFromSockaddr(sa_server, addrRemote);
		info_printf("Received connect response from server: %c from %u.%u.%u.%u:%u.", connectAcknowledge, GPK_IPV4_EXPAND(addrRemote));
		::gpk::sleep(1000);
		gpk_safe_closesocket(handle);
	}
	::gpk::tcpipShutdown();
	return 0;
}
