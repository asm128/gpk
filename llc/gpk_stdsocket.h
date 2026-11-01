#include "gpk_auto_handler.h"
#include "gpk_safe.h"
#include "gpk_tcpip.h"

#ifndef GPK_ATMEL		

#if defined(GPK_WINDOWS)
#	include <WinSock2.h>
#else
	#include <sys/types.h>          /* See NOTES */
	#include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

#ifndef GPK_STDSOCKET_H_23627
#define GPK_STDSOCKET_H_23627

#if !defined(GPK_WINDOWS)
typedef int SOCKET;
#	ifndef INVALID_SOCKET
#		define INVALID_SOCKET -1
#	endif
#endif

namespace gpk
{
	struct auto_socket_close : public ::gpk::auto_handler<SOCKET, INVALID_SOCKET>					{
		using			TWrapper						::auto_handler;
		inline											~auto_socket_close					()	noexcept	{ close(); }
		inline			void							close								()	noexcept	{ if(Handle != INVALID_SOCKET) { closesocket(Handle); Handle = INVALID_SOCKET; } }
	};

	gpk::err_t 		tcpipAddressToSockaddr		(u0_t a1, u0_t a2, u0_t a3, u0_t a4, u1_t port, sockaddr_in & sockaddr);
	gpk::err_t 		tcpipAddressToSockaddr		(u2_t address, u1_t port, sockaddr_in & sockaddr);

	gpk::err_t 		tcpipAddressFromSockaddr	(const sockaddr_in & sockaddr, u0_t* a1, u0_t* a2, u0_t* a3, u0_t* a4, u1_t* port);
	gpk::err_t 		tcpipAddressFromSockaddr	(const sockaddr_in & sockaddr, u2_t & address, u1_t & port);
	gpk::err_t 		tcpipAddress				(SOCKET socket, u0_t* a1, u0_t* a2, u0_t* a3, u0_t* a4, u1_t* port);

	stin	gpk::err_t tcpipAddressToSockaddr		(const ::gpk::SIPv4End & addr, sockaddr_in & sockaddr)	{ return tcpipAddressToSockaddr		
		( ::gpk::byte_at(addr.IP, 0)
		, ::gpk::byte_at(addr.IP, 1)
		, ::gpk::byte_at(addr.IP, 2)
		, ::gpk::byte_at(addr.IP, 3)
		, addr.Port, sockaddr
		); 
	}
	stin	gpk::err_t tcpipAddressFromSockaddr	(const sockaddr_in& sockaddr, ::gpk::SIPv4End & address)	{ u0_t addr[4] = {}; if_fail_fe(tcpipAddressFromSockaddr	(sockaddr	, &addr[0], &addr[1], &addr[2], &addr[3], &address.Port)); address.IP = SIPv4{addr[0], addr[1], addr[2], addr[3]}; return 0; }
	stin	gpk::err_t tcpipAddress				(SOCKET socket, ::gpk::SIPv4End & address)					{ u0_t addr[4] = {}; if_fail_fe(tcpipAddress				(socket		, &addr[0], &addr[1], &addr[2], &addr[3], &address.Port)); address.IP = SIPv4{addr[0], addr[1], addr[2], addr[3]}; return 0; }
} // namespace


#endif // GPK_STDSOCKET_H_23627

#endif // GPK_ATMEL
