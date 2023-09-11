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
		inline			void							close								()	noexcept	{ gpk_safe_closesocket(Handle); }
	};

	::gpk::error_t			tcpipAddressToSockaddr		(uint8_t a1, uint8_t a2, uint8_t a3, uint8_t a4, uint16_t port, sockaddr_in & sockaddr);
	::gpk::error_t			tcpipAddressToSockaddr		(uint32_t address, uint16_t port, sockaddr_in & sockaddr);

	::gpk::error_t			tcpipAddressFromSockaddr	(const sockaddr_in & sockaddr, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4, uint16_t* port);
	::gpk::error_t			tcpipAddressFromSockaddr	(const sockaddr_in & sockaddr, uint32_t & address, uint16_t & port);
	::gpk::error_t			tcpipAddress				(SOCKET socket, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4, uint16_t* port);

	stainli	::gpk::error_t	tcpipAddressToSockaddr		(const ::gpk::SIPv4End & addr, sockaddr_in & sockaddr)	{ return tcpipAddressToSockaddr		
		( ::gpk::byte_at(addr.IP, 0)
		, ::gpk::byte_at(addr.IP, 1)
		, ::gpk::byte_at(addr.IP, 2)
		, ::gpk::byte_at(addr.IP, 3)
		, addr.Port, sockaddr
		); 
	}
	stainli	::gpk::error_t	tcpipAddressFromSockaddr	(const sockaddr_in& sockaddr, ::gpk::SIPv4End & address)	{ uint8_t addr[4] = {}; gpk_necs(tcpipAddressFromSockaddr	(sockaddr	, &addr[0], &addr[1], &addr[2], &addr[3], &address.Port)); address.IP = SIPv4{addr[0], addr[1], addr[2], addr[3]}; return 0; }
	stainli	::gpk::error_t	tcpipAddress				(SOCKET socket, ::gpk::SIPv4End & address)					{ uint8_t addr[4] = {}; gpk_necs(tcpipAddress				(socket		, &addr[0], &addr[1], &addr[2], &addr[3], &address.Port)); address.IP = SIPv4{addr[0], addr[1], addr[2], addr[3]}; return 0; }
} // namespace


#endif // GPK_STDSOCKET_H_23627

#endif // GPK_ATMEL
