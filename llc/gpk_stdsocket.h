#include "gpk_auto_handler.h"
#include "gpk_safe.h"
#include "gpk_tcpip.h"

#if defined(GPK_WINDOWS)
#	include <WinSock2.h>
#else
	#include <sys/types.h>          /* See NOTES */
	#include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

#ifndef GPK_STDSOCKET_H_9287349823
#define GPK_STDSOCKET_H_9287349823


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

					::gpk::error_t					tcpipAddressToSockaddr				(const uint8_t* a1, const uint8_t* a2, const uint8_t* a3, const uint8_t* a4, const uint16_t* port, sockaddr_in & sockaddr);
					::gpk::error_t					tcpipAddressFromSockaddr			(const sockaddr_in& sockaddr, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4, uint16_t* port);
					::gpk::error_t					tcpipAddress						(SOCKET socket, uint8_t* a1, uint8_t* a2, uint8_t* a3, uint8_t* a4, uint16_t* port);

	stainli	::gpk::error_t					tcpipAddressToSockaddr				(const ::gpk::SIPv4 & addr, sockaddr_in & sockaddr)	{ return tcpipAddressToSockaddr		(&addr.IP[0], &addr.IP[1], &addr.IP[2], &addr.IP[3], &addr.Port, sockaddr); }
	stainli	::gpk::error_t					tcpipAddressFromSockaddr			(const sockaddr_in& sockaddr, ::gpk::SIPv4 & addr)	{ return tcpipAddressFromSockaddr	(sockaddr	, &addr.IP[0], &addr.IP[1], &addr.IP[2], &addr.IP[3], &addr.Port); }
	stainli	::gpk::error_t					tcpipAddress						(SOCKET socket, ::gpk::SIPv4 & addr)				{ return tcpipAddress				(socket		, &addr.IP[0], &addr.IP[1], &addr.IP[2], &addr.IP[3], &addr.Port); }
} // namespace

#endif // GPK_STDSOCKET_H_9287349823
