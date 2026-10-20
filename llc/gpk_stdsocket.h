#include "gpk_auto_handler.h"
#include "gpk_safe.h"
#include "gpk_tcpip.h"

#ifndef GPK_ATMEL		

#ifdef GPK_WINDOWS
#	include <WinSock2.h>
//#elif defined(GPK_ESP8266)
//#	include <lwip/sockets.h>
#else
#	if defined(GPK_ESP8266)
//#		include <lwip/sockets.h>
#		include <lwip/inet.h>
#	else
#		include <sys/types.h>
#		include <sys/socket.h>
#		include <netinet/in.h>
#	endif
#	include <unistd.h>
#endif

#ifndef GPK_STDSOCKET_H_23627
#define GPK_STDSOCKET_H_23627

namespace gpk
{
#ifndef GPK_WINDOWS
tydf int SOCKET;
#	ifndef INVALID_SOCKET
#		define INVALID_SOCKET -1
#	endif
#endif

	struct auto_socket_close : public ::gpk::auto_handler<SOCKET, INVALID_SOCKET>					{
		using			TWrapper						::auto_handler;
		inline											~auto_socket_close					()	noexcept	{ close(); }
		inline			void							close								()	noexcept	{ if(INVALID_SOCKET != Handle) { closesocket(Handle); Handle = INVALID_SOCKET; } }
	};

	::gpk::error_t			tcpipAddressToSockaddr		(u0_t a1, u0_t a2, u0_t a3, u0_t a4, ::gpk::u1_t port, sockaddr_in & sockaddr);
	::gpk::error_t			tcpipAddressToSockaddr		(::gpk::u2_t address, ::gpk::u1_t port, sockaddr_in & sockaddr);

	//::gpk::error_t			tcpipAddressFromSockaddr	(const sockaddr_in & sockaddr, u0_t * a1, u0_t * a2, u0_t * a3, u0_t * a4, ::gpk::u1_t * port);
	::gpk::error_t			tcpipAddressFromSockaddr	(const sockaddr_in & sockaddr, ::gpk::u2_t & address, ::gpk::u1_t & port);
	::gpk::error_t			tcpipAddress				(SOCKET socket, ::gpk::u2_t & ip, ::gpk::u1_t & port);

	stin	::gpk::error_t	tcpipAddressToSockaddr		(const ::gpk::SIPv4End & addr, sockaddr_in & sockaddr)	{ return tcpipAddressToSockaddr		
		( ::gpk::byte_at(addr.IP, 0)
		, ::gpk::byte_at(addr.IP, 1)
		, ::gpk::byte_at(addr.IP, 2)
		, ::gpk::byte_at(addr.IP, 3)
		, addr.Port, sockaddr
		); 
	}
	stin	::gpk::error_t	tcpipAddressFromSockaddr	(const sockaddr_in & sockaddr, ::gpk::SIPv4End & address)	{ return tcpipAddressFromSockaddr	(sockaddr	, address.IP, address.Port); }
	stin	::gpk::error_t	tcpipAddress				(SOCKET socket, ::gpk::SIPv4End & address)					{ return tcpipAddress				(socket		, address.IP, address.Port); }
} // namespace


#endif // GPK_STDSOCKET_H_23627

#endif // GPK_ATMEL


